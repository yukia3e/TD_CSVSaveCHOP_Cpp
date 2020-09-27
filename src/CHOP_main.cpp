#if OPERATOR_TYPE_CHOP == 1

#include "CPlusPlus_Common.h"
#include "CHOP_CPlusPlusBase.h"

#include <assert.h>
#include <linalg.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <vector>

////

class PROJECT_NAME : public CHOP_CPlusPlusBase
{
public:

	PROJECT_NAME(const OP_NodeInfo* info)
	{
        isFirstWriting = 0;
    }

	virtual ~PROJECT_NAME()
	{}

    void getGeneralInfo(CHOP_GeneralInfo* info) override
    {
		info->cookEveryFrame = true;
		info->timeslice = true;
    }

	bool getOutputInfo(CHOP_OutputInfo* info) override
	{
		return false;
	}

	void execute(const CHOP_Output* output, OP_Inputs* inputs, void*) override
	{
        if (inputs->getNumInputs() == 0)
            return;

        if (inputs == nullptr)
            return;

        int status = inputs->getParInt("Rec");
        const char* filename = inputs->getParString("Filename");

        if (status == 1)
        {
            const OP_CHOPInput *cinput = inputs->getInputCHOP(0);

            if (isFirstWriting == 0)
            {
                std::stringstream headerLine;
                for (int i = 0; i < output->numChannels; i++)
                {
                    if (i != 0) {
                        headerLine << ",";
                    }
                    headerLine << cinput->getChannelName(i);
                }

                std::ofstream ofs(filename);
                ofs << headerLine.str() << std::endl;

                isFirstWriting++;
            }

            std::stringstream contentsLine;
            for (int i = 0; i < output->numChannels; i++)
            {
                const float value = cinput->getChannelData(i)[0];
                output->channels[i][0] = value;

                if (i != 0) {
                    contentsLine << ",";
                }

                if (i < 4) {
                    contentsLine << int(value);
                } else {
                    contentsLine << value;
                }
            }

            std::ofstream ofs(filename, std::ios::app);
            ofs << contentsLine.str() << std::endl;
        }
    }

    void setupParameters(OP_ParameterManager* manager) override
    {
        {
            OP_NumericParameter	np;

            np.name = "Init";
            np.label = "Init";

            OP_ParAppendResult res = manager->appendPulse(np);
            assert(res == OP_ParAppendResult::Success);
        }

        {
            OP_NumericParameter	np;

            np.name = "Rec";
            np.label = "Rec";

            OP_ParAppendResult res = manager->appendToggle(np);
            assert(res == OP_ParAppendResult::Success);
        }

        {
            OP_StringParameter	sp;

            sp.name = "Filename";
            sp.label = "Filename";

            sp.defaultValue = "Slave01_sequenceData.csv";

            OP_ParAppendResult res = manager->appendString(sp);
            assert(res == OP_ParAppendResult::Success);
        }
    }

    void pulsePressed(const char* name) override
    {
        if (!strcmp(name, "Init"))
        {
            isFirstWriting = 0;
        }
    }


private:
    int32_t	isFirstWriting;
};

////

extern "C"
{
	DLLEXPORT int32_t GetCHOPAPIVersion(void)
	{
		return CHOP_CPLUSPLUS_API_VERSION;
	}

	DLLEXPORT CHOP_CPlusPlusBase* CreateCHOPInstance(const OP_NodeInfo* info)
	{
		return new PROJECT_NAME(info);
	}

	DLLEXPORT void DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
	{
		delete (PROJECT_NAME*)instance;
	}
};

#endif
