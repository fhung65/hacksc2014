#define _USE_MATH_DEFINES
#include<iostream>
#include<cmath>
#include<algorithm>
#include<myo/myo.hpp>
#include<OVR_CAPI.h>
#include<OVR_Math.h>
#include<math.h>

///////////////////////////////////////
//Tank functions
///////////////////////////////////////
void tank_forward(int speed)
{
	std::cout << "tf:" << speed - 9 << std::endl;
}

void tank_right(int speed)
{
	std::cout << "tr:" << speed << std::endl;
}

void tank_left(int speed)
{
	std::cout << "tl:" << speed << std::endl;
}

//void tank_back(int speed)
//{
//	std::cout << "tb:" << speed << std::endl;
//}

///////////////////////////////////////
//Camera Functions
///////////////////////////////////////

void cam_rot(float speed)// will be +/- and magnitude matters
{
	std::cout << "cr:" << speed << std::endl;
}

void cam_screenshot()
{
	std::cout << "sc" << std::endl;
}

///////////////////////////////////////
//Myo
///////////////////////////////////////
class tank_listener : public myo::DeviceListener
{
public:
	// These values are set by onArmRecognized() and onArmLost() above.
	bool onArm;
	myo::Arm whichArm;
	// These values are set by onOrientationData() and onPose() above.
	int pitch_w, yaw_w;
	myo::Pose currentPose;

	tank_listener() :
		onArm(false), pitch_w(0), yaw_w(0), currentPose() {}
						
	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		// We've lost a Myo.
		// Let's clean up some leftover state.
		pitch_w = 0;
		yaw_w = 0;
		onArm = false;
	}
	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;
		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
		// Convert the floating point angles in radians to a scale from 0 to 18.
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
	}
	// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
	{
		currentPose = pose;
		// Vibrate the Myo whenever we've detected that the user has made a fist.
		if (pose == myo::Pose::fist) {
			myo->vibrate(myo::Myo::vibrationMedium);
		}
	}
	// onArmRecognized() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
	// arm. This lets Myo know which arm it's on and which way it's facing.
	void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
	{
		onArm = true;
		whichArm = arm;
	}
	// onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
	// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
	// when Myo is moved around on the arm.
	void onArmLost(myo::Myo* myo, uint64_t timestamp)
	{
		onArm = false;
	}


};

////////////////////////////////////////////////
//Oculus
////////////////////////////////////////////////

void init_OVR()
{
	ovr_Initialize();
}

void quit_OVR()
{
	ovr_Shutdown();
}

///////////////////////////////////////////////////
//Main
///////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	std::cout << "starting\n";

	//Initialize head mount device
	std::cout << "Initializing OVR\n";
	ovr_Initialize();

	ovrHmd hmd = ovrHmd_Create(0);
	if(!hmd)
	{
		std::cout << "failed to create head mount device!\n";
		exit(1);
	}
	
	//configure ovrHmd for tracking -> we have three suppotrted tracking capabilities, one req'd
	bool ovrHmd_config_tracking_success = 
		ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation|
									ovrTrackingCap_Position |
									ovrTrackingCap_MagYawCorrection, 0);
	if (!ovrHmd_config_tracking_success)
	{
		std::cout << "failed to configure required capabilities (MagYawCorrection)\n";
		exit(1);
	}
	
	//initialize myo hub 
	myo::Hub hub;

	std::cout << "Trying to find a Myo...\n";
	myo::Myo* myo = hub.waitForMyo(10000);
	
	if (myo == nullptr)
	{	
		std::cout << "Unable to find a Myo!\n";
		exit(1);
	}
	std::cout << "Connected to Myo\n";
	
	//create default listener (data collector)
	tank_listener list;
	//add it to the hub
	hub.addListener(&list);

	int last_shot = 0;
	bool quit = false;
	while (!quit)
	{
		//update the hub, 1 sec/ 20 times 
		hub.run(1000 / 20);
		//get data from myolistener
		myo::Pose myoPose = list.currentPose;
		myo::Arm myoArm = list.whichArm;


		//do stuff with myo data
		if (myoPose == myo::Pose::fist)
		{
			tank_forward(list.pitch_w); // currently, has values 0 - 18
		}

		else if ((myoPose == myo::Pose::waveIn && myoArm == myo::Arm::armLeft)||
				(myoPose == myo::Pose::waveOut && myoArm == myo::Arm::armRight))
		{
			tank_right(18-list.yaw_w);
		}
		else if ((myoPose == myo::Pose::waveIn && myoArm == myo::Arm::armRight) ||
			(myoPose == myo::Pose::waveOut && myoArm == myo::Arm::armLeft))
		{
			tank_left(list.yaw_w);
		}
		else if (myoPose == myo::Pose::thumbToPinky)
		{
			quit = true;
		}
		else if (myoPose == myo::Pose::fingersSpread && last_shot > 50)
		{
			cam_screenshot();
			last_shot = 0;
		}
		if (last_shot < 200)
			last_shot++;

		ovrTrackingState trackState = ovrHmd_GetTrackingState(hmd,
			ovr_GetTimeInSeconds());
		OVR::Quatf ovrPose = trackState.HeadPose.ThePose.Orientation;
		float yaw, pitch, roll;
		ovrPose.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);

		cam_rot(yaw);
	}

	ovrHmd_Destroy(hmd);
	ovr_Shutdown();

	return 0;
}