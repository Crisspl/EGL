#include <EGL/egl.h>

#include <dlfcn.h>

static volatile EGLint g_initializedDisplayCount = 0;
static void* g_libegl = NULL;

typedef EGLBoolean (*PFNEGLCHOOSECONFIG) (EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config);
typedef EGLBoolean (*PFNEGLCOPYBUFFERS) (EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target);
typedef EGLContext (*PFNEGLCREATECONTEXT) (EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint* attrib_list);
typedef EGLSurface (*PFNEGLCREATEPBUFFERSURFACE) (EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list);
typedef EGLSurface (*PFNEGLCREATEPIXMAPSURFACE) (EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list);
typedef EGLSurface (*PFNEGLCREATEWINDOWSURFACE) (EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list);
typedef EGLBoolean (*PFNEGLDESTROYCONTEXT) (EGLDisplay dpy, EGLContext ctx);
typedef EGLBoolean (*PFNEGLDESTROYSURFACE) (EGLDisplay dpy, EGLSurface surface);
typedef EGLBoolean (*PFNEGLGETCONFIGATTRIB) (EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value);
typedef EGLBoolean (*PFNEGLGETCONFIGS) (EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config);
typedef EGLDisplay (*PFNEGLGETCURRENTDISPLAY) (void);
typedef EGLSurface (*PFNEGLGETCURRENTSURFACE) (EGLint readdraw);
typedef EGLDisplay (*PFNEGLGETDISPLAY) (EGLNativeDisplayType display_id);
typedef EGLint (*PFNEGLGETERROR) (void);
typedef __eglMustCastToProperFunctionPointerType (*PFNEGLGETPROCADDRESS) (const char*);
typedef EGLBoolean (*PFNEGLINITIALIZE) (EGLDisplay dpy, EGLint* major, EGLint* minor);
typedef EGLBoolean (*PFNEGLMAKECURRENT) (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
typedef EGLBoolean (*PFNEGLQUERYCONTEXT) (EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value);
typedef const char* (*PFNEGLQUERYSTRING) (EGLDisplay, EGLint);
typedef EGLBoolean (*PFNEGLQUERYSURFACE) (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value);
typedef EGLBoolean (*PFNEGLSWAPBUFFERS) (EGLDisplay dpy, EGLSurface surface);
typedef EGLBoolean (*PFNEGLTERMINATE) (EGLDisplay dpy);
typedef EGLBoolean (*PFNEGLWAITGL) (void);
typedef EGLBoolean (*PFNEGLWAITNATIVE) (EGLint engine);
typedef EGLBoolean (*PFNEGLBINDTEXIMAGE) (EGLDisplay dpy, EGLSurface surface, EGLint buffer);
typedef EGLBoolean (*PFNEGLRELEASETEXIMAGE) (EGLDisplay dpy, EGLSurface surface, EGLint buffer);
typedef EGLBoolean (*PFNEGLSURFACEATTRIB) (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value);
typedef EGLBoolean (*PFNEGLSWAPINTERVAL) (EGLDisplay dpy, EGLint interval);
typedef EGLBoolean (*PFNEGLBINDAPI) (EGLenum api);
typedef EGLenum (*PFNEGLQUERYAPI) (void);
typedef EGLSurface (*PFNEGLCREATEPBUFFERFROMCLIENTBUFFER) (EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list);
typedef EGLBoolean (*PFNEGLRELEASETHREAD) (void);
typedef EGLBoolean (*PFNEGLWAITCLIENT) (void);
typedef EGLContext (*PFNEGLGETCURRENTCONTEXT) (void);
typedef EGLSync (*PFNEGLCREATESYNC) (EGLDisplay dpy, EGLenum type, const EGLAttrib* attrib_list);
typedef EGLBoolean (*PFNEGLDESTROYSYNC) (EGLDisplay dpy, EGLSync sync);
typedef EGLint (*PFNEGLCLIENTWAITSYNC) (EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout);
typedef EGLBoolean (*PFNEGLGETSYNCATTRIB) (EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib* value);
typedef EGLImage (*PFNEGLCREATEIMAGE) (EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint* attrib_list);
typedef EGLBoolean (*PFNEGLDESTROYIMAGE) (EGLDisplay dpy, EGLImage image);
typedef EGLDisplay (*PFNEGLGETPLATFORMDISPLAY) (EGLenum platform, void* native_display, const EGLAttrib* attrib_list);
typedef EGLSurface (*PFNEGLCREATEPLATFORMWINDOWSURFACE) (EGLDisplay dpy, EGLConfig config, void* native_window, const EGLAttrib* attrib_list);
typedef EGLSurface (*PFNEGLCREATEPLATFORMPIXMAPSURFACE) (EGLDisplay dpy, EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list);
typedef EGLBoolean (*PFNEGLWAITSYNC) (EGLDisplay dpy, EGLSync sync, EGLint flags);

static PFNEGLCHOOSECONFIG eglChooseConfig_PTR = NULL;
static PFNEGLCOPYBUFFERS eglCopyBuffers_PTR = NULL;
static PFNEGLCREATECONTEXT eglCreateContext_PTR = NULL;
static PFNEGLCREATEPBUFFERSURFACE eglCreatePbufferSurface_PTR = NULL;
static PFNEGLCREATEPIXMAPSURFACE eglCreatePixmapSurface_PTR = NULL;
static PFNEGLCREATEWINDOWSURFACE eglCreateWindowSurface_PTR = NULL;
static PFNEGLDESTROYCONTEXT eglDestroyContext_PTR = NULL;
static PFNEGLDESTROYSURFACE eglDestroySurface_PTR = NULL;
static PFNEGLGETCONFIGATTRIB eglGetConfigAttrib_PTR = NULL;
static PFNEGLGETCONFIGS eglGetConfigs_PTR = NULL;
static PFNEGLGETCURRENTDISPLAY eglGetCurrentDisplay_PTR = NULL;
static PFNEGLGETCURRENTSURFACE eglGetCurrentSurface_PTR = NULL;
static PFNEGLGETDISPLAY eglGetDisplay_PTR = NULL;
static PFNEGLGETERROR eglGetError_PTR = NULL;
static PFNEGLGETPROCADDRESS eglGetProcAddress_PTR = NULL;
static PFNEGLINITIALIZE eglInitialize_PTR = NULL;
static PFNEGLMAKECURRENT eglMakeCurrent_PTR = NULL;
static PFNEGLQUERYCONTEXT eglQueryContext_PTR = NULL;
static PFNEGLQUERYSTRING eglQueryString_PTR = NULL;
static PFNEGLQUERYSURFACE eglQuerySurface_PTR = NULL;
static PFNEGLSWAPBUFFERS eglSwapBuffers_PTR = NULL;
static PFNEGLTERMINATE eglTerminate_PTR = NULL;
static PFNEGLWAITGL eglWaitGL_PTR = NULL;
static PFNEGLWAITNATIVE eglWaitNative_PTR = NULL;
static PFNEGLBINDTEXIMAGE eglBindTexImage_PTR = NULL;
static PFNEGLRELEASETEXIMAGE eglReleaseTexImage_PTR = NULL;
static PFNEGLSURFACEATTRIB eglSurfaceAttrib_PTR = NULL;
static PFNEGLSWAPINTERVAL eglSwapInterval_PTR = NULL;
static PFNEGLBINDAPI eglBindAPI_PTR = NULL;
static PFNEGLQUERYAPI eglQueryAPI_PTR = NULL;
static PFNEGLCREATEPBUFFERFROMCLIENTBUFFER eglCreatePbufferFromClientBuffer_PTR = NULL;
static PFNEGLRELEASETHREAD eglReleaseThread_PTR = NULL;
static PFNEGLWAITCLIENT eglWaitClient_PTR = NULL;
static PFNEGLGETCURRENTCONTEXT eglGetCurrentContext_PTR = NULL;
static PFNEGLCREATESYNC eglCreateSync_PTR = NULL;
static PFNEGLDESTROYSYNC eglDestroySync_PTR = NULL;
static PFNEGLCLIENTWAITSYNC eglClientWaitSync_PTR = NULL;
static PFNEGLGETSYNCATTRIB eglGetSyncAttrib_PTR = NULL;
static PFNEGLCREATEIMAGE eglCreateImage_PTR = NULL;
static PFNEGLDESTROYIMAGE eglDestroyImage_PTR = NULL;
static PFNEGLGETPLATFORMDISPLAY eglGetPlatformDisplay_PTR = NULL;
static PFNEGLCREATEPLATFORMWINDOWSURFACE eglCreatePlatformWindowSurface_PTR = NULL;
static PFNEGLCREATEPLATFORMPIXMAPSURFACE eglCreatePlatformPixmapSurface_PTR = NULL;
static PFNEGLWAITSYNC eglWaitSync_PTR = NULL;

static EGLBoolean _initFunctionPointers()
{
	if (g_libegl != NULL)
		return EGL_TRUE;

	if (g_libegl == NULL)
		g_libegl = dlopen("libEGL.so", RTLD_LAZY);
	if (g_libegl == NULL)
		return EGL_FALSE;

#define _LOAD_EGL_FN_PTR(fname, fptr_t) fname ## _PTR = (fptr_t) dlsym(g_libegl, #fname)

	_LOAD_EGL_FN_PTR(eglChooseConfig, PFNEGLCHOOSECONFIG);
	_LOAD_EGL_FN_PTR(eglCopyBuffers, PFNEGLCOPYBUFFERS);
	_LOAD_EGL_FN_PTR(eglCreateContext, PFNEGLCREATECONTEXT);
	_LOAD_EGL_FN_PTR(eglCreatePbufferSurface, PFNEGLCREATEPBUFFERSURFACE);
	_LOAD_EGL_FN_PTR(eglCreatePixmapSurface, PFNEGLCREATEPIXMAPSURFACE);
	_LOAD_EGL_FN_PTR(eglCreateWindowSurface, PFNEGLCREATEWINDOWSURFACE);
	_LOAD_EGL_FN_PTR(eglDestroyContext, PFNEGLDESTROYCONTEXT);
	_LOAD_EGL_FN_PTR(eglDestroySurface, PFNEGLDESTROYSURFACE);
	_LOAD_EGL_FN_PTR(eglGetConfigAttrib, PFNEGLGETCONFIGATTRIB);
	_LOAD_EGL_FN_PTR(eglGetConfigs, PFNEGLGETCONFIGS);
	_LOAD_EGL_FN_PTR(eglGetCurrentDisplay, PFNEGLGETCURRENTDISPLAY);
	_LOAD_EGL_FN_PTR(eglGetCurrentSurface, PFNEGLGETCURRENTSURFACE);
	_LOAD_EGL_FN_PTR(eglGetDisplay, PFNEGLGETDISPLAY);
	_LOAD_EGL_FN_PTR(eglGetError, PFNEGLGETERROR);
	_LOAD_EGL_FN_PTR(eglGetProcAddress, PFNEGLGETPROCADDRESS);
	_LOAD_EGL_FN_PTR(eglInitialize, PFNEGLINITIALIZE);
	_LOAD_EGL_FN_PTR(eglMakeCurrent, PFNEGLMAKECURRENT);
	_LOAD_EGL_FN_PTR(eglQueryContext, PFNEGLQUERYCONTEXT);
	_LOAD_EGL_FN_PTR(eglQueryString, PFNEGLQUERYSTRING);
	_LOAD_EGL_FN_PTR(eglQuerySurface, PFNEGLQUERYSURFACE);
	_LOAD_EGL_FN_PTR(eglSwapBuffers, PFNEGLSWAPBUFFERS);
	_LOAD_EGL_FN_PTR(eglTerminate, PFNEGLTERMINATE);
	_LOAD_EGL_FN_PTR(eglWaitGL, PFNEGLWAITGL);
	_LOAD_EGL_FN_PTR(eglWaitNative, PFNEGLWAITNATIVE);
	_LOAD_EGL_FN_PTR(eglBindTexImage, PFNEGLBINDTEXIMAGE);
	_LOAD_EGL_FN_PTR(eglReleaseTexImage, PFNEGLRELEASETEXIMAGE);
	_LOAD_EGL_FN_PTR(eglSurfaceAttrib, PFNEGLSURFACEATTRIB);
	_LOAD_EGL_FN_PTR(eglSwapInterval, PFNEGLSWAPINTERVAL);
	_LOAD_EGL_FN_PTR(eglBindAPI, PFNEGLBINDAPI);
	_LOAD_EGL_FN_PTR(eglQueryAPI, PFNEGLQUERYAPI);
	_LOAD_EGL_FN_PTR(eglCreatePbufferFromClientBuffer, PFNEGLCREATEPBUFFERFROMCLIENTBUFFER);
	_LOAD_EGL_FN_PTR(eglReleaseThread, PFNEGLRELEASETHREAD);
	_LOAD_EGL_FN_PTR(eglWaitClient, PFNEGLWAITCLIENT);

#define _LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(fname, fptr_t) \
	_LOAD_EGL_FN_PTR(fname, fptr_t);\
	if (fname ## _PTR == NULL)\
		return EGL_FALSE

	// EGL >=1.3 functions:
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglGetCurrentContext, PFNEGLGETCURRENTCONTEXT);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglCreateSync, PFNEGLCREATESYNC);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglDestroySync, PFNEGLDESTROYSYNC);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglClientWaitSync, PFNEGLCLIENTWAITSYNC);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglGetSyncAttrib, PFNEGLGETSYNCATTRIB);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglCreateImage, PFNEGLCREATEIMAGE);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglDestroyImage, PFNEGLDESTROYIMAGE);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglGetPlatformDisplay, PFNEGLGETPLATFORMDISPLAY);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglCreatePlatformWindowSurface, PFNEGLCREATEPLATFORMWINDOWSURFACE);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglCreatePlatformPixmapSurface, PFNEGLCREATEPLATFORMPIXMAPSURFACE);
	_LOAD_EGL_FN_PTR_RETURN_ON_FAILURE(eglWaitSync, PFNEGLWAITSYNC);

#undef _LOAD_EGL_FN_PTR_RETURN_ON_FAILURE
#undef _LOAD_EGL_FN_PTR

	return EGL_TRUE;
}

EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint* attrib_list, EGLConfig* configs, EGLint config_size, EGLint* num_config)
{
	return eglChooseConfig_PTR(dpy, attrib_list, configs, config_size, num_config);
}

EGLAPI EGLBoolean EGLAPIENTRY eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target)
{
	return eglCopyBuffers_PTR(dpy, surface, target);
}

EGLAPI EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint* attrib_list)
{
	return eglCreateContext_PTR(dpy, config, share_context, attrib_list);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint* attrib_list)
{
	return eglCreatePbufferSurface_PTR(dpy, config, attrib_list);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint* attrib_list)
{
	return eglCreatePixmapSurface_PTR(dpy, config, pixmap, attrib_list);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint* attrib_list)
{
	return eglCreateWindowSurface_PTR(dpy, config, win, attrib_list);
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
	return eglDestroyContext_PTR(dpy, ctx);
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
	return eglDestroySurface_PTR(dpy, surface);
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint* value)
{
	return eglGetConfigAttrib_PTR(dpy, config, attribute, value);
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigs(EGLDisplay dpy, EGLConfig* configs, EGLint config_size, EGLint* num_config)
{
	return eglGetConfigs_PTR(dpy, configs, config_size, num_config);
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetCurrentDisplay(void)
{
	return eglGetCurrentDisplay_PTR();
}

EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
	return eglGetCurrentSurface_PTR(readdraw);
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType display_id)
{
	if (!_initFunctionPointers())
		return EGL_NO_DISPLAY;

	return eglGetDisplay_PTR(display_id);
}

EGLAPI EGLint EGLAPIENTRY eglGetError(void)
{
	return eglGetError_PTR();
}

EGLAPI __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress(const char* procname)
{
	return eglGetProcAddress_PTR(procname);
}

EGLAPI EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint* major, EGLint* minor)
{
	EGLBoolean retval = eglInitialize_PTR(dpy, major, minor);
	if (retval)
		++g_initializedDisplayCount;
	return retval;
}

EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	return eglMakeCurrent_PTR(dpy, draw, read, ctx);
}

EGLAPI EGLBoolean EGLAPIENTRY eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint* value)
{
	return eglQueryContext_PTR(dpy, ctx, attribute, value);
}

EGLAPI const char* EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name)
{
	return eglQueryString_PTR(dpy, name);
}

EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint* value)
{
	return eglQuerySurface_PTR(dpy, surface, attribute, value);
}

EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	return eglSwapBuffers_PTR(dpy, surface);
}

EGLAPI EGLBoolean EGLAPIENTRY eglTerminate(EGLDisplay dpy)
{
	EGLBoolean retval = eglTerminate_PTR(dpy);
	if (retval)
		--g_initializedDisplayCount;

	if (g_initializedDisplayCount == 0 && g_libegl)
	{
		dlclose(g_libegl);
		g_libegl = NULL;
	}

	return retval;
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitGL(void)
{
	return eglWaitGL_PTR();
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitNative(EGLint engine)
{
	return eglWaitNative_PTR(engine);
}

//
// EGL_VERSION_1_1
//

EGLAPI EGLBoolean EGLAPIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	return eglBindTexImage_PTR(dpy, surface, buffer);
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
	return eglReleaseTexImage_PTR(dpy, surface, buffer);
}

EGLAPI EGLBoolean EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
	return eglSurfaceAttrib_PTR(dpy, surface, attribute, value);
}

EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	return eglSwapInterval_PTR(dpy, interval);
}

//
// EGL_VERSION_1_2
//

EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum api)
{
	return eglBindAPI_PTR(api);
}

EGLAPI EGLenum EGLAPIENTRY eglQueryAPI(void)
{
	return eglQueryAPI_PTR();
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint* attrib_list)
{
	return eglCreatePbufferFromClientBuffer_PTR(dpy, buftype, buffer, config, attrib_list);
}

EGLAPI EGLBoolean EGLAPIENTRY eglReleaseThread(void)
{
	return eglReleaseThread_PTR();
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitClient(void)
{
	return eglWaitClient_PTR();
}

//
// EGL_VERSION_1_3
//

//
// EGL_VERSION_1_4
//

EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext(void)
{
	return eglGetCurrentContext_PTR();
}

//
// EGL_VERSION_1_5
//

EGLAPI EGLSync EGLAPIENTRY eglCreateSync(EGLDisplay dpy, EGLenum type, const EGLAttrib* attrib_list)
{
	return eglCreateSync_PTR(dpy, type, attrib_list);
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroySync(EGLDisplay dpy, EGLSync sync)
{
	return eglDestroySync_PTR(dpy, sync);
}

EGLAPI EGLint EGLAPIENTRY eglClientWaitSync(EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout)
{
	return eglClientWaitSync_PTR(dpy, sync, flags, timeout);
}

EGLAPI EGLBoolean EGLAPIENTRY eglGetSyncAttrib(EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib* value)
{
	return eglGetSyncAttrib_PTR(dpy, sync, attribute, value);
}

EGLAPI EGLImage EGLAPIENTRY eglCreateImage(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint* attrib_list)
{
	return eglCreateImage_PTR(dpy, ctx, target, buffer, attrib_list);
}

EGLAPI EGLBoolean EGLAPIENTRY eglDestroyImage(EGLDisplay dpy, EGLImage image)
{
	return eglDestroyImage_PTR(dpy, image);
}

EGLAPI EGLDisplay EGLAPIENTRY eglGetPlatformDisplay(EGLenum platform, void* native_display, const EGLAttrib* attrib_list)
{
	return eglGetPlatformDisplay_PTR(platform, native_display, attrib_list);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePlatformWindowSurface(EGLDisplay dpy, EGLConfig config, void* native_window, const EGLAttrib* attrib_list)
{
	return eglCreatePlatformWindowSurface_PTR(dpy, config, native_window, attrib_list);
}

EGLAPI EGLSurface EGLAPIENTRY eglCreatePlatformPixmapSurface(EGLDisplay dpy, EGLConfig config, void* native_pixmap, const EGLAttrib* attrib_list)
{
	return eglCreatePlatformPixmapSurface_PTR(dpy, config, native_pixmap, attrib_list);
}

EGLAPI EGLBoolean EGLAPIENTRY eglWaitSync(EGLDisplay dpy, EGLSync sync, EGLint flags)
{
	return eglWaitSync_PTR(dpy, sync, flags);
}