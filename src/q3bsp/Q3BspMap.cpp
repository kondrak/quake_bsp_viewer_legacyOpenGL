#include <algorithm>
#include <sstream>
#include "Application.hpp"
#include "q3bsp/Q3BspMap.hpp"
#include "q3bsp/Q3BspPatch.hpp"
#include "renderer/OpenGLHeaders.hpp"
#include "renderer/Texture.hpp"
#include "Frustum.hpp"
#include "renderer/CameraDirector.hpp"
#include "renderer/Math.hpp"

extern Application g_application;
extern CameraDirector g_cameraDirector;
extern Frustum g_frustum;

static const int tesselationLevel = 10;   // level of curved surface tesselation
static const float worldScale     = 64.f; // scale-down factor for the map


Q3BspMap::~Q3BspMap()
{
    delete [] entities.ents;
    delete [] visData.vecs;
    delete [] m_lightmapTextures;

    for(std::vector<Q3BspPatch *>::iterator it = m_patches.begin(); it != m_patches.end(); ++it)
        delete *it;

    for(std::vector<Texture *>::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
        delete *it;

    delete m_missingTex;
}


void Q3BspMap::Init()
{
    m_missingTex = new Texture( "res/missing.png" );

    // load textures
    LoadTextures();

    // load lightmaps
    LoadLightmaps();

    // create renderable faces and patches
    m_renderLeaves.reserve( leaves.size() );

    for(std::vector<Q3BspLeafLump>::iterator it = leaves.begin(); it != leaves.end(); ++it)
    {
        m_renderLeaves.push_back( Q3LeafRenderable() );
        m_renderLeaves.back().visCluster = (*it).cluster;
        m_renderLeaves.back().firstFace  = (*it).leafFace;
        m_renderLeaves.back().numFaces   = (*it).n_leafFaces;

        // create a bounding box
        m_renderLeaves.back().boundingBoxVertices[0] = Math::Vector3f( (float)(*it).mins.x, (float)(*it).mins.y, (float)(*it).mins.z );
        m_renderLeaves.back().boundingBoxVertices[1] = Math::Vector3f( (float)(*it).mins.x, (float)(*it).mins.y,(float)(*it).maxs.z );
        m_renderLeaves.back().boundingBoxVertices[2] = Math::Vector3f( (float)(*it).mins.x, (float)(*it).maxs.y,(float)(*it).mins.z );
        m_renderLeaves.back().boundingBoxVertices[3] = Math::Vector3f( (float)(*it).mins.x, (float)(*it).maxs.y,(float)(*it).maxs.z );
        m_renderLeaves.back().boundingBoxVertices[4] = Math::Vector3f( (float)(*it).maxs.x, (float)(*it).mins.y,(float)(*it).mins.z );
        m_renderLeaves.back().boundingBoxVertices[5] = Math::Vector3f( (float)(*it).maxs.x, (float)(*it).mins.y,(float)(*it).maxs.z );
        m_renderLeaves.back().boundingBoxVertices[6] = Math::Vector3f( (float)(*it).maxs.x, (float)(*it).maxs.y,(float)(*it).mins.z );
        m_renderLeaves.back().boundingBoxVertices[7] = Math::Vector3f( (float)(*it).maxs.x, (float)(*it).maxs.y,(float)(*it).maxs.z );

        for(int j = 0; j < 8; ++j)
        {
            m_renderLeaves.back().boundingBoxVertices[j].m_x /= worldScale;
            m_renderLeaves.back().boundingBoxVertices[j].m_y /= worldScale;
            m_renderLeaves.back().boundingBoxVertices[j].m_z /= worldScale;
        } 
    }

    m_renderFaces.reserve( faces.size() );

    int faceArrayIdx  = 0;
    int patchArrayIdx = 0;

    for(std::vector<Q3BspFaceLump>::iterator it = faces.begin(); it != faces.end(); it++)
    {
        m_renderFaces.push_back( Q3FaceRenderable() );

        //is it a patch?
        if( (*it).type == FaceTypePatch)  
        {
            m_renderFaces.back().index = patchArrayIdx;
            CreatePatch( *it );
            ++patchArrayIdx;
        }
        else
        {
            m_renderFaces.back().index = faceArrayIdx;
        }

        ++faceArrayIdx;
        m_renderFaces.back().type = (*it).type;
    }

    m_mapStats.totalVertices = vertices.size();
    m_mapStats.totalFaces    = faces.size();
    m_mapStats.totalPatches  = patchArrayIdx;
}


void Q3BspMap::OnRenderStart()
{ 
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    // Texture0 - the "regular" texture
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Texture1 - lightmap texture
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);

    // make lightmaps more vivid/colorful
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
    glTexEnvi(GL_TEXTURE_ENV,  GL_SOURCE0_RGB_EXT, GL_PREVIOUS_EXT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV,  GL_COMBINE_RGB_EXT, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV,  GL_SOURCE1_RGB_EXT, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV,    GL_RGB_SCALE_EXT, 2.0f);

    // switch back to texture 0 - this is what we want to render first
    glActiveTextureARB(GL_TEXTURE0_ARB);
}


void Q3BspMap::Render()
{ 
    m_mapStats.visiblePatches = 0;

    if( HasRenderFlag( Q3RenderShowWireframe ) )
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    else
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // determine which faces are visible
    CalculateVisibleFaces( g_cameraDirector.GetActiveCamera()->Position() * worldScale );

    // scale down the entire world!
    glScalef( 1.f / worldScale, 1.f / worldScale, 1.f / worldScale);

    // render faces
    for(std::vector<Q3FaceRenderable *>::iterator it = m_visibleFaces.begin(); it != m_visibleFaces.end(); it++)
    {
        // polygons and meshes are rendered in the same manner
        if( (*it)->type == FaceTypePolygon || (*it)->type == FaceTypeMesh ) 
        {           
            RenderFace( &faces[ (*it)->index ] );
        }

        // render all biquad patches that compose the curved surface
        if( (*it)->type == FaceTypePatch)
        {
            RenderPatch( (*it)->index );
            m_mapStats.visiblePatches++;
        }
    }

    glDisable(GL_TEXTURE_2D);
}


void Q3BspMap::OnRenderFinish()
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}


// determine if a bsp cluster is visible from a given camera cluster
bool Q3BspMap::ClusterVisible(int cameraCluster, int testCluster) const
{
    if ((visData.vecs == NULL) || (cameraCluster < 0)) {
        return true;
    }

    int idx = (cameraCluster * visData.sz_vecs) + (testCluster >> 3);

    return (visData.vecs[idx] & (1 << (testCluster & 7))) != 0;
}


// determine which bsp leaf camera resides in
int Q3BspMap::FindCameraLeaf(const Math::Vector3f & cameraPosition) const
{
    int leafIndex = 0;

    while(leafIndex >= 0)
    {
        // children.x - front node; children.y - back node
        if(PointPlanePos( planes[nodes[leafIndex].plane].normal.x,
                          planes[nodes[leafIndex].plane].normal.y,
                          planes[nodes[leafIndex].plane].normal.z,
                          planes[nodes[leafIndex].plane].dist, 
                          cameraPosition ) == Math::PointInFrontOfPlane )
        {
            leafIndex = nodes[leafIndex].children.x; 
        }
        else
        {
            leafIndex = nodes[leafIndex].children.y;
        }
    }

    return ~leafIndex;
}


//Calculate which faces to draw given a camera position & view frustum
void Q3BspMap::CalculateVisibleFaces(const Math::Vector3f &cameraPosition)
{
    //Clear the list of faces drawn
    m_visibleFaces.clear();

    //calculate the camera leaf
    int cameraLeaf    = FindCameraLeaf(cameraPosition);
    int cameraCluster = m_renderLeaves[cameraLeaf].visCluster;

    //loop through the leaves
    for(std::vector<Q3LeafRenderable>::iterator it = m_renderLeaves.begin(); it != m_renderLeaves.end(); ++it)
    {
        //if the leaf is not in the PVS - skip it
        if( !HasRenderFlag( Q3RenderSkipPVS ) && !ClusterVisible(cameraCluster, (*it).visCluster) )
            continue;

        //if this leaf does not lie in the frustum - skip it
        if( !HasRenderFlag( Q3RenderSkipFC ) && !g_frustum.BoxInFrustum(  (*it).boundingBoxVertices ) )
            continue;

        //loop through faces in this leaf and them to visibility set
        for(int j = 0; j < (*it).numFaces; ++j)
        {
            int idx = leafFaces[ (*it).firstFace + j ].face;
            Q3FaceRenderable *face = &m_renderFaces[ leafFaces[ (*it).firstFace + j ].face ];

            if(std::find( m_visibleFaces.begin(), m_visibleFaces.end(), face ) == m_visibleFaces.end() )
                m_visibleFaces.push_back( face );
        }
    }

    m_mapStats.visibleFaces = m_visibleFaces.size();
}


void Q3BspMap::LoadTextures()
{
    // get number of textures
    int numTextures = header.direntries[Textures].length / sizeof(Q3BspTextureLump);

    m_textures.resize( numTextures );

    // load the textures from file (determine wheter it's a jpg or tga)
    for(std::vector<Q3BspFaceLump>::iterator it = faces.begin(); it != faces.end(); it++)       
    {
        if(m_textures[ it->texture ])
            continue;

        std::string nameJPG = textures[ it->texture ].name;

        nameJPG.append(".jpg");

        m_textures[ it->texture ] = new Texture( nameJPG.c_str() );
        int ret = m_textures[ it->texture ]->Load();

        if(ret == 0)
        {
            std::string nameTGA = textures[ it->texture ].name;
            nameTGA.append(".tga");

            delete m_textures[ it->texture ];
            m_textures[ it->texture ] = new Texture( nameTGA.c_str() );
            ret = m_textures[ it->texture ]->Load();

            if(ret == 0)
            {
                std::stringstream sstream;
                sstream << "Missing texture: " << nameTGA.c_str() << "\n";
                g_application.LogError( sstream.str().c_str() );
            }
        }
    }
}


void Q3BspMap::LoadLightmaps()
{
    m_lightmapTextures=new GLuint[ lightMaps.size() ];

    glGenTextures( lightMaps.size(), m_lightmapTextures);

    // optional: change gamma settings of the lightmaps (make them brighter)
    SetLightmapGamma(2.5f);

    for(size_t i = 0; i < lightMaps.size(); ++i)
    {
        glBindTexture(GL_TEXTURE_2D, m_lightmapTextures[i]);

        // Create texture from bsp lmap data
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, lightMaps[i].map);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    // Create white texture for if no lightmap specified
    float white[] = {1.0f, 1.0f, 1.0f, 1.0f };

    glGenTextures(1, &m_whiteTex);
    glBindTexture(GL_TEXTURE_2D, m_whiteTex);

    //Create texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, 1, 1, GL_RGB, GL_FLOAT, white);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,     GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,     GL_TEXTURE_WRAP_T, GL_REPEAT);
}


// tweak lightmap gamma settings
void Q3BspMap::SetLightmapGamma(float gamma)
{
    for(size_t i = 0; i < lightMaps.size(); ++i)
    {
        for(int j = 0; j < 128*128; ++j)
        {
            float r, g, b;

            r = lightMaps[i].map[ j*3+0 ];
            g = lightMaps[i].map[ j*3+1 ];
            b = lightMaps[i].map[ j*3+2 ];

            r *= gamma / 255.0f;
            g *= gamma / 255.0f;
            b *= gamma / 255.0f;

            float scale = 1.0f;
            float temp;
            if( r > 1.0f && (temp = (1.0f/r) ) < scale) scale = temp;
            if( g > 1.0f && (temp = (1.0f/g) ) < scale) scale = temp;
            if( b > 1.0f && (temp = (1.0f/b) ) < scale) scale = temp;

            scale *= 255.0f;
            r *= scale;
            g *= scale;
            b *= scale;

            lightMaps[i].map[ j*3+0 ] = (GLubyte)r;
            lightMaps[i].map[ j*3+1 ] = (GLubyte)g;
            lightMaps[i].map[ j*3+2 ] = (GLubyte)b;
        }
    }
}


// create a Q3Bsp curved surface
void Q3BspMap::CreatePatch(const Q3BspFaceLump &f)
{
    Q3BspPatch *newPatch = new Q3BspPatch;

    newPatch->textureIdx  = f.texture;
    newPatch->lightmapIdx = f.lm_index;
    newPatch->width  = f.size.x;
    newPatch->height = f.size.y;

    int numPatchesWidth  = ( newPatch->width - 1  ) >> 1;
    int numPatchesHeight = ( newPatch->height - 1 ) >> 1;

    newPatch->quadraticPatches.resize( numPatchesWidth*numPatchesHeight );

    // generate biquadratic patches (components that make the curved surface)
    for(int y = 0; y < numPatchesHeight; ++y)
    {
        for(int x = 0; x < numPatchesWidth; ++x)
        {
            for(int row = 0; row < 3; ++row)
            {
                for(int col = 0; col < 3; ++col)
                {
                    int patchIdx = y * numPatchesWidth + x;
                    int cpIdx = row * 3 + col;
                    newPatch->quadraticPatches[ patchIdx ].controlPoints[ cpIdx ] = vertices[ f.vertex +
                                                                                              ( y * 2 * newPatch->width + x * 2 ) +
                                                                                              row * newPatch->width + col ];
                }
            }

            newPatch->quadraticPatches[ y * numPatchesWidth + x ].Tesselate( tesselationLevel );
        }
    }

    m_patches.push_back(newPatch);
}


// render regular faces (polygons + meshes)
void Q3BspMap::RenderFace(const Q3BspFaceLump *f)
{
    int stride = sizeof(Q3BspVertexLump);

    glVertexPointer( 3, GL_FLOAT, stride, &(vertices[ f->vertex ].position) );

    // set texture coords
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    glTexCoordPointer( 2, GL_FLOAT, stride, &(vertices[ f->vertex ].texcoord[0]) );

    if(m_textures[ f->texture ]->Load() > 0 )
        glBindTexture(GL_TEXTURE_2D, m_textures[ f->texture ]->Load());          
    else
    {
        if( HasRenderFlag( Q3RenderSkipMissingTex ) )
        {
            return;
        }

        glBindTexture(GL_TEXTURE_2D, m_missingTex->Load());
    }

    // texture has alpha channel - alpha test it if proper render flag is set
    if( HasRenderFlag(Q3RenderAlphaTest) && (m_textures[ f->texture ]->Components() == 4) )
    {
        glAlphaFunc(GL_GREATER, 0.05f);
        glEnable(GL_ALPHA_TEST);

        glDisable(GL_CULL_FACE);
    } 

    // disable Tex0 if we want to render lightmaps only
    if( HasRenderFlag(Q3RenderShowLightmaps) )
    {
        glActiveTexture(GL_TEXTURE0);
        glDisable(GL_TEXTURE_2D);
    }
    else if( !HasRenderFlag(Q3RenderUseLightmaps) )
    {
        glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);
    }


    // set lightmap coords
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glTexCoordPointer(2, GL_FLOAT, stride, &(vertices[ f->vertex ].texcoord[1]));

    glActiveTextureARB(GL_TEXTURE1_ARB);
    // bind a generic white texture if there's no lightmap for this surface
    if( f->lm_index >= 0 )
        glBindTexture(GL_TEXTURE_2D, m_lightmapTextures[ f->lm_index ]);
    else
        glBindTexture(GL_TEXTURE_2D, m_whiteTex);
   

    // render the face + blend it with lightmap (if it's enabled)
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDrawElements(GL_TRIANGLES, f->n_meshverts,
        GL_UNSIGNED_INT, &meshVertices[f->meshvert]);


    if( HasRenderFlag(Q3RenderAlphaTest) && (m_textures[ f->texture ]->Components() == 4) )
    {
        glEnable(GL_CULL_FACE);
        glDisable(GL_ALPHA_TEST);
    }
}


// render curved surface
void Q3BspMap::RenderPatch(int idx)
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, m_textures[ m_patches[idx]->textureIdx ]->Load() );

    glActiveTextureARB(GL_TEXTURE1_ARB);

    // bind a generic white texture if there's no lightmap for this surface
    if( m_patches[idx]->lightmapIdx >= 0 ) 
        glBindTexture(GL_TEXTURE_2D, m_lightmapTextures[ m_patches[idx ]->lightmapIdx]);
    else
        glBindTexture(GL_TEXTURE_2D, m_whiteTex);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    int numPatches = m_patches[idx]->quadraticPatches.size();

    for(int i = 0; i< numPatches; ++i)
        m_patches[idx]->quadraticPatches[i].Render( HasRenderFlag(Q3RenderShowLightmaps) );
}
