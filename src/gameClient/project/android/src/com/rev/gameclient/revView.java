////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 10th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// android rev view
package com.rev.gameclient;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

class revView extends GLSurfaceView
{
	public revView(Context context)
	{
		// Super class construction
		super(context);
		// Set gl context version to 2.0
		setEGLContextClientVersion(2);
		// Set renderer
		setRenderer(new revRenderer());
	}
}

//------------------------------------------------------------------------------------------------------------------
//-- rev renderer
//------------------------------------------------------------------------------------------------------------------
class revRenderer implements GLSurfaceView.Renderer {
	
	// Nativee interface
	public static native void updateGameClient	();
	// TODO: Accept different screen sizes
	public static native void setScreenSize		();
	
 public void onDrawFrame(GL10 gl) {
 	updateGameClient();
 }

 public void onSurfaceChanged(GL10 gl, int width, int height) {
 	setScreenSize();
 }

 public void onSurfaceCreated(GL10 gl, EGLConfig config) {
     // Do nothing.
 }
}