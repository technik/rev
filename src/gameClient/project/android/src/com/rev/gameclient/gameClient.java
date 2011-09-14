////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 10th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// android game client
package com.rev.gameclient;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;

public class gameClient extends Activity
{
	// Load the shared library
	static
	{
		System.loadLibrary("nativeGameClient");
	}

	public static native void initGameClient();
	public static native void endGameClient();
	
	revView mView;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
		initGameClient();
        mView = new revView(this);
        setContentView(mView);
    }
    
    @Override
    protected void onPause()
    {
    	super.onPause();
    	mView.onPause();
    }
    
    @Override
    protected void onResume()
    {
    	super.onResume();
    	mView.onResume();
    }

	@Override
	public void onDestroy()
	{
		endGameClient();
		super.onDestroy();
	}
}
