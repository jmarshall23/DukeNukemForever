// DnAI_SchoolGirl.h
//

//
// DnSchoolGirl
//
class DnSchoolGirl : public DnAI
{
	CLASS_PROTOTYPE(DnSchoolGirl);
public:
	stateResult_t				state_Begin(stateParms_t* parms);
	stateResult_t				state_Idle(stateParms_t* parms);
};