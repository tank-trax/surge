#include "DualDelayEffect.h"
#include "ChorusEffectImpl.h"
#include "CombulatorEffect.h"
#include "Eq3BandEffect.h"
#include "GEQ11Effect.h"
#include "FreqshiftEffect.h"
#include "PhaserEffect.h"
#include "RotarySpeakerEffect.h"
#include "DistortionEffect.h"
#include "VocoderEffect.h"
#include "ConditionerEffect.h"
#include "Reverb1Effect.h"
#include "Reverb2Effect.h"
#include "FlangerEffect.h"
#include "ResonatorEffect.h"
#include "RingModulatorEffect.h"
#include "airwindows/AirWindowsEffect.h"
#include "chowdsp/Neuron.h"
#include "DebugHelpers.h"

using namespace std;

Effect *spawn_effect(int id, SurgeStorage *storage, FxStorage *fxdata, pdata *pd)
{
    // std::cout << "Spawn Effect " << _D(id) << std::endl;
    // Surge::Debug::stackTraceToStdout(7);
    switch (id)
    {
    case fxt_delay:
        return new DualDelayEffect(storage, fxdata, pd);
    case fxt_eq:
        return new Eq3BandEffect(storage, fxdata, pd);
    case fxt_phaser:
        return new PhaserEffect(storage, fxdata, pd);
    case fxt_rotaryspeaker:
        return new RotarySpeakerEffect(storage, fxdata, pd);
    case fxt_distortion:
        return new DistortionEffect(storage, fxdata, pd);
    case fxt_reverb:
        return new Reverb1Effect(storage, fxdata, pd);
    case fxt_reverb2:
        return new Reverb2Effect(storage, fxdata, pd);
    case fxt_freqshift:
        return new FreqshiftEffect(storage, fxdata, pd);
    case fxt_conditioner:
        return new ConditionerEffect(storage, fxdata, pd);
    case fxt_chorus4:
        return new ChorusEffect<4>(storage, fxdata, pd);
    case fxt_vocoder:
        return new VocoderEffect(storage, fxdata, pd);
    case fxt_flanger:
        return new FlangerEffect(storage, fxdata, pd);
    case fxt_ringmod:
        return new RingModulatorEffect(storage, fxdata, pd);
    case fxt_airwindows:
        return new AirWindowsEffect(storage, fxdata, pd);
    case fxt_neuron:
        return new chowdsp::Neuron(storage, fxdata, pd);
    case fxt_geq11:
        return new GEQ11Effect(storage, fxdata, pd);
    case fxt_resonator:
        return new ResonatorEffect(storage, fxdata, pd);
    case fxt_combulator:
        return new CombulatorEffect(storage, fxdata, pd);
    case fxt_chow:
    case fxt_exciter:
    case fxt_ensemble:
    case fxt_nimbus:
    default:
        return 0;
    };
}

Effect::Effect(SurgeStorage *storage, FxStorage *fxdata, pdata *pd)
{
    // assert(storage);
    this->fxdata = fxdata;
    this->storage = storage;
    this->pd = pd;
    ringout = 10000000;
    if (pd)
    {
        for (int i = 0; i < n_fx_params; i++)
        {
            f[i] = &pd[fxdata->p[i].id].f;
            pdata_ival[i] = &pd[fxdata->p[i].id].i;
        }
    }
}

bool Effect::process_ringout(float *dataL, float *dataR, bool indata_present)
{
    if (indata_present)
        ringout = 0;
    else
        ringout++;

    int d = get_ringout_decay();
    if ((d < 0) || (ringout < d) || (ringout == 0))
    {
        process(dataL, dataR);
        return true;
    }
    else
        process_only_control();
    return false;
}

void Effect::init_ctrltypes()
{
    for (int j = 0; j < n_fx_params; j++)
    {
        fxdata->p[j].modulateable = true;
        fxdata->p[j].set_type(ct_none);
    }
}
