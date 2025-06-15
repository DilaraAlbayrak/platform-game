#pragma once
#include "D3DFramework.h"

class Game final {
private:
    D3DFramework& _d3dFramework;

public:
	Game() : _d3dFramework(D3DFramework::getInstance()) {}

    Game(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&) = delete;

    HRESULT initWindow(HINSTANCE hInstance, int nCmdShow) {
        return _d3dFramework.initWindow(hInstance, nCmdShow);
    }

    HRESULT initDevice()
    {		
        return _d3dFramework.initDevice();
    }

    void render()
    {
        _d3dFramework.render();
    }

    void reset()
    {
		_d3dFramework.reset();
    }
};