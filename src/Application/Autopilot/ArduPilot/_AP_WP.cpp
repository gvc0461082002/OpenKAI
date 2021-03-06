#include "../ArduPilot/_AP_WP.h"

namespace kai
{

_AP_WP::_AP_WP()
{
	m_pAP = NULL;
	m_pDS = NULL;
	m_dZdefault = 0.0;
	m_kZsensor = 1.0;
	m_apMount.init();
}

_AP_WP::~_AP_WP()
{
}

bool _AP_WP::init(void* pKiss)
{
	IF_F(!this->_AutopilotBase::init(pKiss));
	Kiss* pK = (Kiss*) pKiss;

	pK->v("dZdefault",&m_dZdefault);
	pK->v("kZsensor",&m_kZsensor);

	Kiss* pG = pK->o("mount");
	if(!pG->empty())
	{
		double p=0, r=0, y=0;
		pG->v("pitch", &p);
		pG->v("roll", &r);
		pG->v("yaw", &y);

		m_apMount.m_control.input_a = p * 100;	//pitch
		m_apMount.m_control.input_b = r * 100;	//roll
		m_apMount.m_control.input_c = y * 100;	//yaw
		m_apMount.m_control.save_position = 0;

		pG->v("stabPitch", &m_apMount.m_config.stab_pitch);
		pG->v("stabRoll", &m_apMount.m_config.stab_roll);
		pG->v("stabYaw", &m_apMount.m_config.stab_yaw);
		pG->v("mountMode", &m_apMount.m_config.mount_mode);
	}

	string iName;

	iName = "";
	pK->v("APcopter_base", &iName);
	m_pAP = (_AP_base*) (pK->parent()->getChildInst(iName));
	IF_Fl(!m_pAP, iName + ": not found");

	iName = "";
	pK->v("_DistSensorBase", &iName);
	m_pDS = (_DistSensorBase*) (pK->root()->getChildInst(iName));

	return true;
}

int _AP_WP::check(void)
{
	NULL__(m_pAP,-1);
	NULL__(m_pAP->m_pMavlink,-1);

	return this->_AutopilotBase::check();
}

void _AP_WP::update(void)
{
	this->_AutopilotBase::update();
	IF_(check()<0);
	IF_(!bActive());
	Waypoint* pWP = (Waypoint*)m_pMC->getCurrentMission();
	NULL_(pWP);

	m_pAP->setMount(m_apMount);
/*
	double alt = (double)(m_pAP->m_pMavlink->m_msg.global_position_int.relative_alt) * 1e-3;
	if(m_pDS)
	{
		double dS = m_pDS->dAvr();
		if(dS > 0)
			alt += (pWP->m_vWP.z - dS) * m_kZsensor;
		else
			alt += m_dZdefault;
	}
	else
	{
		alt = pWP->m_vWP.z;
	}

	mavlink_set_position_target_global_int_t spt;
	spt.coordinate_frame = MAV_FRAME_GLOBAL_RELATIVE_ALT_INT;

	//target position
	spt.lat_int = pWP->m_vWP.x*1e7;
	spt.lon_int = pWP->m_vWP.y*1e7;
	spt.alt = (float)alt;

	//velocity, ignored at the moment
	spt.vx = 0.0;//(float)pWP->m_speedH;		//forward
	spt.vy = 0.0;//(float)pWP->m_speedH;		//right
	spt.vz = 0.0;//(float)vZ;					//down

	//heading
	spt.yaw_rate = (float)180.0 * DEG_RAD;
	spt.yaw = m_pAP->m_pMavlink->m_msg.attitude.yaw;
	if(pWP->m_hdg >= 0)
		spt.yaw = (float)pWP->m_hdg * DEG_RAD;

	spt.type_mask = 0b0000000111111000;

	m_pAP->m_pMavlink->setPositionTargetGlobalINT(spt);
	*/
}

void _AP_WP::draw(void)
{
	this->_AutopilotBase::draw();

	if(!bActive())
		addMsg("Inactive",1);
	else
		addMsg("Waypoint",1);
}

}
