#pragma once

namespace rt
{

class EditOP
{
public:
	virtual void OnKeyDown(int key_code) {}
	virtual void OnKeyUp(int key_code) {}

	virtual void OnMouseLeftDown(float x, float y) {}
	virtual void OnMouseLeftUp(float x, float y) {}
	virtual void OnMouseRightDown(float x, float y) {}
	virtual void OnMouseRightUp(float x, float y) {}
	virtual void OnMouseMove(float x, float y) {}
	virtual void OnMouseDrag(float x, float y) {}
	virtual void OnMouseWheelRotation(float x, float y, float offx, float offy) {}

}; // EditOP

}