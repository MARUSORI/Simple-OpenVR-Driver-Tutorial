#include "VirtualCompositor.hpp"
#include <Debug.hpp>

VirtualCompositor::VirtualCompositor() {
	_serial = "vc_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

void VirtualCompositor::GetWindowBounds(int32_t * x, int32_t * y, uint32_t * width, uint32_t * height)
{
	*x = _display_properties.display_offset_x;
	*y = _display_properties.display_offset_y;
	*width = _display_properties.display_width;
	*height = _display_properties.display_height;
}

bool VirtualCompositor::IsDisplayOnDesktop()
{
	return false;
}

bool VirtualCompositor::IsDisplayRealDisplay()
{
	return true;
}

void VirtualCompositor::GetRecommendedRenderTargetSize(uint32_t * width, uint32_t * height)
{// Use the stored display properties to return the render target size
	*width = _display_properties.render_width;
	*height = _display_properties.render_height;
}

void VirtualCompositor::GetEyeOutputViewport(vr::EVREye eye, uint32_t * x, uint32_t * y, uint32_t * width, uint32_t * height)
{
	*y = _display_properties.display_offset_y;
	*width = _display_properties.render_width / 2;
	*height = _display_properties.render_height;

	if (eye == vr::EVREye::Eye_Left) {
		*x = _display_properties.display_offset_x;
	}
	else {
		*x = _display_properties.display_offset_x + _display_properties.render_width / 2;
	}
}

void VirtualCompositor::GetProjectionRaw(vr::EVREye eEye, float * left, float * right, float * top, float * bottom)
{
	*left = -1;
	*right = 1;
	*top = -1;
	*bottom = 1;
}

vr::DistortionCoordinates_t VirtualCompositor::ComputeDistortion(vr::EVREye eEye, float u, float v)
{
	vr::DistortionCoordinates_t coordinates;
	coordinates.rfBlue[0] = u;
	coordinates.rfBlue[1] = v;
	coordinates.rfGreen[0] = u;
	coordinates.rfGreen[1] = v;
	coordinates.rfRed[0] = u;
	coordinates.rfRed[1] = v;
	return coordinates;
}

VirtualCompositor::~VirtualCompositor() {
	
}

std::shared_ptr<VirtualCompositor> VirtualCompositor::make_new()
{
	return std::shared_ptr<VirtualCompositor>(new VirtualCompositor());
}

std::string VirtualCompositor::get_serial() const
{
	return _serial;
}

void VirtualCompositor::update()
{

}

vr::TrackedDeviceIndex_t VirtualCompositor::get_index() const
{
	return _index;
}

void VirtualCompositor::process_event(const vr::VREvent_t & event)
{
}



vr::EVRInitError VirtualCompositor::Activate(vr::TrackedDeviceIndex_t index)
{

	_index = index;
	_render_thread.start(_serial, _display_properties.display_width, _display_properties.display_height, true);

	// Get the properties handle
	_props = vr::VRProperties()->TrackedDeviceToPropertyContainer(_index);

	// Set some universe ID (Must be 2 or higher)
	vr::VRProperties()->SetUint64Property(_props, vr::Prop_CurrentUniverseId_Uint64, 2);

	// Set the IPD to be whatever steam has configured
	vr::VRProperties()->SetFloatProperty(_props, vr::Prop_UserIpdMeters_Float, vr::VRSettings()->GetFloat(vr::k_pch_SteamVR_Section, vr::k_pch_SteamVR_IPD_Float));

	// Set the display FPS
	vr::VRProperties()->SetFloatProperty(_props, vr::Prop_DisplayFrequency_Float, 90.f);

	// Disable warnings about compositor not being fullscreen
	vr::VRProperties()->SetBoolProperty(_props, vr::Prop_IsOnDesktop_Bool, true);
	
	return vr::VRInitError_None;
}

void VirtualCompositor::Deactivate()
{
	_render_thread.stop(true);
	// Clear device id
	_index = vr::k_unTrackedDeviceIndexInvalid;
}

void VirtualCompositor::EnterStandby()
{
}

void * VirtualCompositor::GetComponent(const char * component)
{
	if (std::string(component) == std::string(vr::IVRVirtualDisplay_Version)){
		return static_cast<vr::IVRVirtualDisplay*>(this);
	}
	if (std::string(component) == std::string(vr::IVRDisplayComponent_Version)){
		return static_cast<vr::IVRDisplayComponent*>(this);
	}
	return nullptr;
}

void VirtualCompositor::DebugRequest(const char * request, char * response_buffer, uint32_t response_buffer_size)
{
	// No custom debug requests defined
	if (response_buffer_size >= 1)
		response_buffer[0] = 0;
}

vr::DriverPose_t VirtualCompositor::GetPose()
{
	vr::DriverPose_t pose = { 0 };
	pose.poseIsValid = true;
	pose.result = vr::TrackingResult_Running_OK;
	pose.deviceIsConnected = true;
	pose.qWorldFromDriverRotation.w = 1;
	pose.qWorldFromDriverRotation.x = 0;
	pose.qWorldFromDriverRotation.y = 0;
	pose.qWorldFromDriverRotation.z = 0;
	pose.qDriverFromHeadRotation.w = 1;
	pose.qDriverFromHeadRotation.x = 0;
	pose.qDriverFromHeadRotation.y = 0;
	pose.qDriverFromHeadRotation.z = 0;
	return pose;
}

void VirtualCompositor::Present(const vr::PresentInfo_t * present_info, uint32_t present_info_size)
{
	_render_thread.draw_texture(present_info, _display_properties.render_width, _display_properties.render_height, false);
	
}

void VirtualCompositor::WaitForPresent()
{
}

bool VirtualCompositor::GetTimeSinceLastVsync(float * seconds_since_last_vsync, uint64_t * frame_counter)
{

	return false;
}
