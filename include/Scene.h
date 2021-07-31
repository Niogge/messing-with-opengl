#pragma once
class Camera;
class Window;
class IScene{
public:
    virtual void init() = 0;
    virtual void draw(Camera& c, Window& win) =0;
};