//*****************
//RASTERIZER STATES
//*****************

RasterizerState gRS_FrontCulling
{
    FillMode = Solid;
    CullMode = FRONT;
};

RasterizerState gRS_BackCulling
{
    FillMode = Solid;
    CullMode = BACK;
};

RasterizerState gRS_NoCulling
{
    FillMode = Solid;
    CullMode = NONE;
};

//**************
//SAMPLER STATES
//**************
SamplerState gSS_LinearWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
};

SamplerState gSS_PointWrap
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
};

//********************
//DEPTH STENCIL STATES
//********************
DepthStencilState gDSS_DepthNoWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = 0;
};

DepthStencilState gDSS_NoDepthNoWrite
{
    DepthEnable = FALSE;
    DepthWriteMask = 0;
};

DepthStencilState gDSS_EnableDepthAndWrite
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

DepthStencilState gDSS_EnableDepthAndWriteLessEqual
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

//************
//BLEND STATES
//************
BlendState gBS_EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

BlendState gBS_NoBlending
{
    BlendEnable[0] = FALSE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};
