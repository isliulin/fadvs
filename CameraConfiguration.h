#pragma once

#include <pylon/ConfigurationEventHandler.h>
using namespace GenApi;
class CCameraConfiguration : public Pylon::CCameraEventHandler
{
public:
	void OnOpened(Pylon::CInstantCamera& camera)
	{
		try
		{
			INodeMap &control = camera.GetNodeMap();

			const CIntegerPtr width = control.GetNode("Width");
			const CIntegerPtr height = control.GetNode("Height");
			const CIntegerPtr offsetX = control.GetNode("OffsetX");
			const CIntegerPtr offsetY = control.GetNode("OffsetY");

			if(IsWritable(offsetX))
			{
				offsetX->SetValue(offsetX->GetMin());
			}
			if(IsWritable(offsetY))
			{
				offsetY->SetValue(offsetY->GetMin());
			}
			width->SetValue(width->GetMax());
			height->SetValue(height->GetMax());

			const CIntegerPtr packetSize1394 = control.GetNode("PacketSize");
			if(IsWritable(packetSize1394))
			{
				packetSize1394->SetValue(4096);
			}
			const CIntegerPtr packetSizeGigE = control.GetNode("GevSCPSPacketSize");
			if(IsWritable(packetSizeGigE))
			{
				packetSizeGigE->SetValue(1500);
			}
		}
		catch (GenICam::GenericException& e)
		{
			throw RUNTIME_EXCEPTION("Could not apply configuration. GenICam::GenericException caught in OnOpen method msg=%hs", e.what());
		}
	}
};