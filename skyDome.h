#pragma once



#include "main.h"

#include "renderer.h"

#include "model.h"



class SkyDome

{

protected:

    XMFLOAT3        Position;

    XMFLOAT3        Scale;

    XMFLOAT3        Rotate;

    int             TexID;

    MODEL*          Model;

public:

    HRESULT InitPolygonModel(void);

    void FinalizePolygonModel(void);

    void UpdatePolygonModel(void);

    void DrawPolygonModel(void);

};



