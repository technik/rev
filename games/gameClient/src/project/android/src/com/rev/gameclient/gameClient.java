////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 10th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// android game client
package com.rev.gameclient;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.opengl.GLSurfaceView;
import android.os.Bundle;

public class gameClient extends Activity
{
	// Load the shared library
	static
	{
		System.loadLibrary("nativeGameClient");
	}
	
	revView mView;
	revFileManager mFileManager;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
		mFileManager = new revFileManager(this);
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
		mView.destroyGame();
		super.onDestroy();
	}
}

class revFileManager
{
	// Constructor
	public revFileManager(Context _context)
	{
		PackageManager packMgmr = _context.getPackageManager();
		ApplicationInfo appInfo = null;
		try
		{
			appInfo = packMgmr.getApplicationInfo("com.rev.gameclient", 0);
		} catch (NameNotFoundException e)
		{
			e.printStackTrace();
			throw new RuntimeException("Unable to locate assets, aborting...");
		}
		initFileSystem(appInfo.sourceDir);
	}

	// Native interface
	public static native void initFileSystem(String _apkPath);
}
