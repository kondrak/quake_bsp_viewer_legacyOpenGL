#ifndef STATSUI_INCLUDED
#define STATSUI_INCLUDED

/*
 * Map stats display UI base class
 */

class BspMap;

class StatsUI
{
public:   
    StatsUI(BspMap *map) : m_map(map)
    {
    }

    virtual ~StatsUI() 
    {
    }

    virtual void OnRenderStart()  = 0;  // prepare for render
    virtual void Render()         = 0;  // perform rendering
    virtual void OnRenderFinish() = 0;  // finish render

protected:
    BspMap *m_map;
};


#endif