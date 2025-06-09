/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define trunc(x) ((Int)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class rnbomatic : public PatcherInterfaceImpl {
public:

rnbomatic()
{
}

~rnbomatic()
{
}

rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = z - y - w + x;
    number f1 = w - x - f0;
    number f2 = y - w;
    number f3 = x;
    return f0 * a * a2 + f1 * a2 + f2 * a + f3;
}

inline number splineinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = -0.5 * w + 1.5 * x - 1.5 * y + 0.5 * z;
    number f1 = w - 2.5 * x + 2 * y - 0.5 * z;
    number f2 = -0.5 * w + 0.5 * y;
    return f0 * a * a2 + f1 * a2 + f2 * a + x;
}

inline number cosT8(number r) {
    number t84 = 56.0;
    number t83 = 1680.0;
    number t82 = 20160.0;
    number t81 = 2.4801587302e-05;
    number t73 = 42.0;
    number t72 = 840.0;
    number t71 = 1.9841269841e-04;

    if (r < 0.785398163397448309615660845819875721 && r > -0.785398163397448309615660845819875721) {
        number rr = r * r;
        return 1.0 - rr * t81 * (t82 - rr * (t83 - rr * (t84 - rr)));
    } else if (r > 0.0) {
        r -= 1.57079632679489661923132169163975144;
        number rr = r * r;
        return -r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    } else {
        r += 1.57079632679489661923132169163975144;
        number rr = r * r;
        return r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    }
}

inline number cosineinterp(number frac, number x, number y) {
    number a2 = (1.0 - this->cosT8(frac * 3.14159265358979323846)) / (number)2.0;
    return x * (1.0 - a2) + y * a2;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number samplerate() {
    return this->sr;
}

Index vectorsize() {
    return this->vs;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number safepow(number base, number exponent) {
    return fixnan(rnbo_pow(base, exponent));
}

number scale(
    number x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow
) {
    auto inscale = this->safediv(1., hiin - lowin);
    number outdiff = highout - lowout;
    number value = (x - lowin) * inscale;

    if (pow != 1) {
        if (value > 0)
            value = this->safepow(value, pow);
        else
            value = -this->safepow(-value, pow);
    }

    value = value * outdiff + lowout;
    return value;
}

number wrap(number x, number low, number high) {
    number lo;
    number hi;

    if (low == high)
        return low;

    if (low > high) {
        hi = low;
        lo = high;
    } else {
        lo = low;
        hi = high;
    }

    number range = hi - lo;

    if (x >= lo && x < hi)
        return x;

    if (range <= 0.000000001)
        return lo;

    long numWraps = (long)(trunc((x - lo) / range));
    numWraps = numWraps - ((x < lo ? 1 : 0));
    number result = x - range * numWraps;

    if (result >= hi)
        return result - range;
    else
        return result;
}

number triangle(number phase, number duty) {
    number p1 = duty;
    auto wrappedPhase = this->wrap(phase, 0., 1.);
    p1 = (p1 > 1. ? 1. : (p1 < 0. ? 0. : p1));

    if (wrappedPhase < p1)
        return wrappedPhase / p1;
    else
        return (p1 == 1. ? wrappedPhase : 1. - (wrappedPhase - p1) / (1. - p1));
}

MillisecondTime currenttime() {
    return this->_currentTime;
}

number tempo() {
    return this->getTopLevelPatcher()->globaltransport_getTempo(this->currenttime());
}

number mstobeats(number ms) {
    return ms * this->tempo() * 0.008 / (number)480;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);

    this->fftstream_tilde_02_perform(
        in1,
        this->fftstream_tilde_02_imagIn,
        this->signals[0],
        this->signals[1],
        this->dummyBuffer,
        n
    );

    this->fftstream_tilde_01_perform(
        in1,
        this->fftstream_tilde_01_imagIn,
        this->signals[2],
        this->signals[3],
        this->signals[4],
        n
    );

    this->gen_02_perform(
        this->signals[4],
        this->gen_02_multype,
        this->gen_02_freqOffset,
        this->gen_02_dutyCycle,
        this->signals[5],
        this->signals[6],
        n
    );

    this->fftstream_tilde_03_perform(
        in2,
        this->fftstream_tilde_03_imagIn,
        this->signals[4],
        this->signals[7],
        this->dummyBuffer,
        n
    );

    this->fftstream_tilde_04_perform(
        in2,
        this->fftstream_tilde_04_imagIn,
        this->signals[8],
        this->signals[9],
        this->dummyBuffer,
        n
    );

    this->gen_01_perform(
        this->signals[2],
        this->signals[3],
        this->signals[0],
        this->signals[1],
        this->signals[4],
        this->signals[7],
        this->signals[8],
        this->signals[9],
        this->signals[5],
        this->signals[6],
        this->gen_01_direction,
        this->signals[10],
        this->signals[11],
        this->signals[12],
        this->signals[13],
        this->signals[14],
        this->signals[15],
        this->signals[16],
        this->signals[17],
        n
    );

    this->ifftstream_tilde_01_perform(
        this->signals[10],
        this->signals[11],
        this->signals[6],
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->dspexpr_01_perform(this->signals[6], this->dspexpr_01_in2, this->signals[11], n);

    this->ifftstream_tilde_02_perform(
        this->signals[12],
        this->signals[13],
        this->signals[6],
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->dspexpr_03_perform(this->signals[6], this->dspexpr_03_in2, this->signals[13], n);
    this->dspexpr_02_perform(this->signals[11], this->signals[13], out1, n);

    this->ifftstream_tilde_03_perform(
        this->signals[14],
        this->signals[15],
        this->signals[13],
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->dspexpr_04_perform(this->signals[13], this->dspexpr_04_in2, this->signals[15], n);

    this->ifftstream_tilde_04_perform(
        this->signals[16],
        this->signals[17],
        this->signals[13],
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->dspexpr_06_perform(this->signals[13], this->dspexpr_06_in2, this->signals[17], n);
    this->dspexpr_05_perform(this->signals[15], this->signals[17], out2, n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 18; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->fftstream_tilde_02_dspsetup(forceDSPSetup);
    this->fftstream_tilde_01_dspsetup(forceDSPSetup);
    this->gen_02_dspsetup(forceDSPSetup);
    this->fftstream_tilde_03_dspsetup(forceDSPSetup);
    this->fftstream_tilde_04_dspsetup(forceDSPSetup);
    this->gen_01_dspsetup(forceDSPSetup);
    this->ifftstream_tilde_01_dspsetup(forceDSPSetup);
    this->ifftstream_tilde_02_dspsetup(forceDSPSetup);
    this->ifftstream_tilde_03_dspsetup(forceDSPSetup);
    this->ifftstream_tilde_04_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->gen_01_del3_bufferobj);
        break;
        }
    case 1:
        {
        return addressOf(this->gen_01_del2_bufferobj);
        break;
        }
    case 2:
        {
        return addressOf(this->gen_01_del1_bufferobj);
        break;
        }
    case 3:
        {
        return addressOf(this->RNBODefaultFftWindow);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 4;
}

void fillDataRef(DataRefIndex , DataRef& ) {}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->gen_01_del3_buffer = new Float64Buffer(this->gen_01_del3_bufferobj);
    }

    if (index == 1) {
        this->gen_01_del2_buffer = new Float64Buffer(this->gen_01_del2_bufferobj);
    }

    if (index == 2) {
        this->gen_01_del1_buffer = new Float64Buffer(this->gen_01_del1_bufferobj);
    }

    if (index == 3) {
        this->fftstream_tilde_01_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
        this->ifftstream_tilde_01_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
        this->fftstream_tilde_02_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
        this->ifftstream_tilde_02_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
        this->fftstream_tilde_03_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
        this->ifftstream_tilde_03_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
        this->fftstream_tilde_04_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
        this->ifftstream_tilde_04_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    }
}

void initialize() {
    this->gen_01_del3_bufferobj = initDataRef("gen_01_del3_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del2_bufferobj = initDataRef("gen_01_del2_bufferobj", true, nullptr, "buffer~");
    this->gen_01_del1_bufferobj = initDataRef("gen_01_del1_bufferobj", true, nullptr, "buffer~");
    this->RNBODefaultFftWindow = initDataRef("RNBODefaultFftWindow", false, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->gen_01_del3_bufferobj->setIndex(0);
    this->gen_01_del3_buffer = new Float64Buffer(this->gen_01_del3_bufferobj);
    this->gen_01_del2_bufferobj->setIndex(1);
    this->gen_01_del2_buffer = new Float64Buffer(this->gen_01_del2_bufferobj);
    this->gen_01_del1_bufferobj->setIndex(2);
    this->gen_01_del1_buffer = new Float64Buffer(this->gen_01_del1_bufferobj);
    this->RNBODefaultFftWindow->setIndex(3);
    this->fftstream_tilde_01_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->ifftstream_tilde_01_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->fftstream_tilde_02_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->ifftstream_tilde_02_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->fftstream_tilde_03_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->ifftstream_tilde_03_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->fftstream_tilde_04_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->ifftstream_tilde_04_win_buf = new Float32Buffer(this->RNBODefaultFftWindow);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "direction"));
    this->param_02_getPresetValue(getSubState(preset, "multype"));
    this->param_03_getPresetValue(getSubState(preset, "freqOffset"));
    this->param_04_getPresetValue(getSubState(preset, "dutyCycle"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "direction"));
    this->param_02_setPresetValue(getSubState(preset, "multype"));
    this->param_03_setPresetValue(getSubState(preset, "freqOffset"));
    this->param_04_setPresetValue(getSubState(preset, "dutyCycle"));
}

void processTempoEvent(MillisecondTime time, Tempo tempo) {
    this->updateTime(time);

    if (this->globaltransport_setTempo(this->_currentTime, tempo, false))
        {}
}

void processTransportEvent(MillisecondTime time, TransportState state) {
    this->updateTime(time);

    if (this->globaltransport_setState(this->_currentTime, state, false))
        {}
}

void processBeatTimeEvent(MillisecondTime time, BeatTime beattime) {
    this->updateTime(time);

    if (this->globaltransport_setBeatTime(this->_currentTime, beattime, false))
        {}
}

void onSampleRateChanged(double ) {}

void processTimeSignatureEvent(MillisecondTime time, int numerator, int denominator) {
    this->updateTime(time);

    if (this->globaltransport_setTimeSignature(this->_currentTime, numerator, denominator, false))
        {}
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 4;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "direction";
        }
    case 1:
        {
        return "multype";
        }
    case 2:
        {
        return "freqOffset";
        }
    case 3:
        {
        return "dutyCycle";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "direction";
        }
    case 1:
        {
        return "multype";
        }
    case 2:
        {
        return "freqOffset";
        }
    case 3:
        {
        return "dutyCycle";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.5;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
    case 1:
    case 3:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 1000 ? 1000 : value));
            ParameterValue normalizedValue = (value - 0) / (1000 - 0);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 0:
    case 1:
    case 3:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1000 - 0);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->gen_01_direction_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->gen_02_multype_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->gen_02_freqOffset_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->gen_02_dutyCycle_set(v);
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 2;
}

Index getNumOutputChannels() const {
    return 2;
}

void allocateDataRefs() {
    this->gen_01_del3_buffer = this->gen_01_del3_buffer->allocateIfNeeded();

    if (this->gen_01_del3_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del3_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del3_bufferobj);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->gen_01_del2_buffer = this->gen_01_del2_buffer->allocateIfNeeded();

    if (this->gen_01_del2_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del2_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del2_bufferobj);

        this->getEngine()->sendDataRefUpdated(1);
    }

    this->gen_01_del1_buffer = this->gen_01_del1_buffer->allocateIfNeeded();

    if (this->gen_01_del1_bufferobj->hasRequestedSize()) {
        if (this->gen_01_del1_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_del1_bufferobj);

        this->getEngine()->sendDataRefUpdated(2);
    }

    this->fftstream_tilde_01_win_buf = this->fftstream_tilde_01_win_buf->allocateIfNeeded();
    this->ifftstream_tilde_01_win_buf = this->ifftstream_tilde_01_win_buf->allocateIfNeeded();
    this->fftstream_tilde_02_win_buf = this->fftstream_tilde_02_win_buf->allocateIfNeeded();
    this->ifftstream_tilde_02_win_buf = this->ifftstream_tilde_02_win_buf->allocateIfNeeded();
    this->fftstream_tilde_03_win_buf = this->fftstream_tilde_03_win_buf->allocateIfNeeded();
    this->ifftstream_tilde_03_win_buf = this->ifftstream_tilde_03_win_buf->allocateIfNeeded();
    this->fftstream_tilde_04_win_buf = this->fftstream_tilde_04_win_buf->allocateIfNeeded();
    this->ifftstream_tilde_04_win_buf = this->ifftstream_tilde_04_win_buf->allocateIfNeeded();

    if (this->RNBODefaultFftWindow->hasRequestedSize()) {
        if (this->RNBODefaultFftWindow->wantsFill())
            this->zeroDataRef(this->RNBODefaultFftWindow);

        this->getEngine()->sendDataRefUpdated(3);
    }
}

void initializeObjects() {
    this->gen_01_del3_init();
    this->gen_01_del2_init();
    this->gen_01_del1_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    this->processParamInitEvents();
}

static number param_01_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_direction_set(number v) {
    this->gen_01_direction = v;
}

static number param_02_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_multype_set(number v) {
    this->gen_02_multype = v;
}

static number param_03_value_constrain(number v) {
    v = (v > 1000 ? 1000 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_freqOffset_set(number v) {
    this->gen_02_freqOffset = v;
}

static number param_04_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_02_dutyCycle_set(number v) {
    this->gen_02_dutyCycle = v;
}

void fftstream_tilde_02_perform(
    const Sample * realIn,
    number imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    RNBO_UNUSED(imagIn);
    auto __fftstream_tilde_02_datapos = this->fftstream_tilde_02_datapos;
    number invFftSize = (number)1 / (number)1024;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        SampleIndex i = (SampleIndex)(__fftstream_tilde_02_datapos);
        array<number, 3> out = {0, 0, i};
        number windowFactor = 1;

        if (__fftstream_tilde_02_datapos < 1024) {
            this->fftstream_tilde_02_inWorkspace[(Index)(2 * i)] = windowFactor * realIn[(Index)i0];
            out[0] = this->fftstream_tilde_02_outWorkspace[(Index)(2 * i)];
            this->fftstream_tilde_02_inWorkspace[(Index)(2 * i + 1)] = windowFactor * 0;
            out[1] = this->fftstream_tilde_02_outWorkspace[(Index)(2 * i + 1)];

            if (i == 1024 - 1) {
                this->fftstream_tilde_02_fft_next(this->fftstream_tilde_02_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->fftstream_tilde_02_outWorkspace[(Index)j] = this->fftstream_tilde_02_inWorkspace[(Index)j];
                }
            }

            __fftstream_tilde_02_datapos = (__fftstream_tilde_02_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->fftstream_tilde_02_datapos = __fftstream_tilde_02_datapos;
}

void fftstream_tilde_01_perform(
    const Sample * realIn,
    number imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    RNBO_UNUSED(imagIn);
    auto __fftstream_tilde_01_datapos = this->fftstream_tilde_01_datapos;
    number invFftSize = (number)1 / (number)1024;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        SampleIndex i = (SampleIndex)(__fftstream_tilde_01_datapos);
        array<number, 3> out = {0, 0, i};
        number windowFactor = 1;

        if (__fftstream_tilde_01_datapos < 1024) {
            this->fftstream_tilde_01_inWorkspace[(Index)(2 * i)] = windowFactor * realIn[(Index)i0];
            out[0] = this->fftstream_tilde_01_outWorkspace[(Index)(2 * i)];
            this->fftstream_tilde_01_inWorkspace[(Index)(2 * i + 1)] = windowFactor * 0;
            out[1] = this->fftstream_tilde_01_outWorkspace[(Index)(2 * i + 1)];

            if (i == 1024 - 1) {
                this->fftstream_tilde_01_fft_next(this->fftstream_tilde_01_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->fftstream_tilde_01_outWorkspace[(Index)j] = this->fftstream_tilde_01_inWorkspace[(Index)j];
                }
            }

            __fftstream_tilde_01_datapos = (__fftstream_tilde_01_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->fftstream_tilde_01_datapos = __fftstream_tilde_01_datapos;
}

void gen_02_perform(
    const Sample * in1,
    number multype,
    number freqOffset,
    number dutyCycle,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    number mul3_2 = this->vectorsize() * 8;
    number rdiv8_5 = (((this->samplerate() == 0. ? 0. : mul3_2 / this->samplerate())) == 0. ? 0. : (number)1 / ((this->samplerate() == 0. ? 0. : mul3_2 / this->samplerate())));
    number expr_2_10 = mul3_2;
    Index i;

    for (i = 0; i < n; i++) {
        number expr1_0 = 0;
        number expr2_1 = 0;
        number gt4_3 = in1[(Index)i] > mul3_2;
        number sub6_4 = gt4_3 * 2 - 1;

        if (multype == 0) {
            expr1_0 = sub6_4;
            expr2_1 = 1;
        } else {
            expr1_0 = 1;
            expr2_1 = sub6_4;
        }

        number mul10_6 = (rdiv8_5 + freqOffset) * expr1_0;
        auto tri12_8 = this->triangle(this->gen_02_phasor_7_next(mul10_6, 0), dutyCycle);
        number expr_1_9 = tri12_8 * expr2_1;
        out1[(Index)i] = expr_1_9;
        out2[(Index)i] = expr_2_10;
    }
}

void fftstream_tilde_03_perform(
    const Sample * realIn,
    number imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    RNBO_UNUSED(imagIn);
    auto __fftstream_tilde_03_datapos = this->fftstream_tilde_03_datapos;
    number invFftSize = (number)1 / (number)1024;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        SampleIndex i = (SampleIndex)(__fftstream_tilde_03_datapos);
        array<number, 3> out = {0, 0, i};
        number windowFactor = 1;

        if (__fftstream_tilde_03_datapos < 1024) {
            this->fftstream_tilde_03_inWorkspace[(Index)(2 * i)] = windowFactor * realIn[(Index)i0];
            out[0] = this->fftstream_tilde_03_outWorkspace[(Index)(2 * i)];
            this->fftstream_tilde_03_inWorkspace[(Index)(2 * i + 1)] = windowFactor * 0;
            out[1] = this->fftstream_tilde_03_outWorkspace[(Index)(2 * i + 1)];

            if (i == 1024 - 1) {
                this->fftstream_tilde_03_fft_next(this->fftstream_tilde_03_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->fftstream_tilde_03_outWorkspace[(Index)j] = this->fftstream_tilde_03_inWorkspace[(Index)j];
                }
            }

            __fftstream_tilde_03_datapos = (__fftstream_tilde_03_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->fftstream_tilde_03_datapos = __fftstream_tilde_03_datapos;
}

void fftstream_tilde_04_perform(
    const Sample * realIn,
    number imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    RNBO_UNUSED(imagIn);
    auto __fftstream_tilde_04_datapos = this->fftstream_tilde_04_datapos;
    number invFftSize = (number)1 / (number)1024;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        SampleIndex i = (SampleIndex)(__fftstream_tilde_04_datapos);
        array<number, 3> out = {0, 0, i};
        number windowFactor = 1;

        if (__fftstream_tilde_04_datapos < 1024) {
            this->fftstream_tilde_04_inWorkspace[(Index)(2 * i)] = windowFactor * realIn[(Index)i0];
            out[0] = this->fftstream_tilde_04_outWorkspace[(Index)(2 * i)];
            this->fftstream_tilde_04_inWorkspace[(Index)(2 * i + 1)] = windowFactor * 0;
            out[1] = this->fftstream_tilde_04_outWorkspace[(Index)(2 * i + 1)];

            if (i == 1024 - 1) {
                this->fftstream_tilde_04_fft_next(this->fftstream_tilde_04_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->fftstream_tilde_04_outWorkspace[(Index)j] = this->fftstream_tilde_04_inWorkspace[(Index)j];
                }
            }

            __fftstream_tilde_04_datapos = (__fftstream_tilde_04_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->fftstream_tilde_04_datapos = __fftstream_tilde_04_datapos;
}

void gen_01_perform(
    const Sample * in1,
    const Sample * in2,
    const Sample * in3,
    const Sample * in4,
    const Sample * in5,
    const Sample * in6,
    const Sample * in7,
    const Sample * in8,
    const Sample * in9,
    const Sample * in10,
    number direction,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    SampleValue * out5,
    SampleValue * out6,
    SampleValue * out7,
    SampleValue * out8,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        number radius1 = 0;
        number angle2 = 0;
        radius1 = (in1[(Index)i] * in1[(Index)i] + in2[(Index)i] * in2[(Index)i] <= 0 ? 0 : rnbo_sqrt(in1[(Index)i] * in1[(Index)i] + in2[(Index)i] * in2[(Index)i])), angle2 = rnbo_atan2(in2[(Index)i], in1[(Index)i]);
        number radius3 = 0;
        number angle4 = 0;
        radius3 = (in3[(Index)i] * in3[(Index)i] + in4[(Index)i] * in4[(Index)i] <= 0 ? 0 : rnbo_sqrt(in3[(Index)i] * in3[(Index)i] + in4[(Index)i] * in4[(Index)i])), angle4 = rnbo_atan2(in4[(Index)i], in3[(Index)i]);
        number radius5 = 0;
        number angle6 = 0;
        radius5 = (in5[(Index)i] * in5[(Index)i] + in6[(Index)i] * in6[(Index)i] <= 0 ? 0 : rnbo_sqrt(in5[(Index)i] * in5[(Index)i] + in6[(Index)i] * in6[(Index)i])), angle6 = rnbo_atan2(in6[(Index)i], in5[(Index)i]);
        number radius7 = 0;
        number angle8 = 0;
        radius7 = (in7[(Index)i] * in7[(Index)i] + in8[(Index)i] * in8[(Index)i] <= 0 ? 0 : rnbo_sqrt(in7[(Index)i] * in7[(Index)i] + in8[(Index)i] * in8[(Index)i])), angle8 = rnbo_atan2(in8[(Index)i], in7[(Index)i]);
        auto scale1_0 = this->scale(direction, 0, 1, 0, in10[(Index)i], 1);
        this->gen_01_del1_write(in9[(Index)i]);
        number tap_1_1 = this->gen_01_del1_read(scale1_0, 0);
        this->gen_01_del3_write(in9[(Index)i]);
        number tap_2_2 = this->gen_01_del3_read(in10[(Index)i], 0);
        this->gen_01_del2_write(tap_2_2);
        number tap_3_3 = this->gen_01_del2_read(scale1_0, 0);
        number mul1_4 = radius1 * tap_3_3;
        number mul2_5 = radius3 * tap_1_1;
        number mul3_6 = radius5 * tap_3_3;
        number mul4_7 = radius7 * tap_1_1;
        number expr_4 = 0;
        number expr_5 = 0;
        expr_4 = mul1_4 * rnbo_cos(angle2), expr_5 = mul1_4 * rnbo_sin(angle2);
        number expr_6 = 0;
        number expr_7 = 0;
        expr_6 = mul2_5 * rnbo_cos(angle4), expr_7 = mul2_5 * rnbo_sin(angle4);
        number expr_8 = 0;
        number expr_9 = 0;
        expr_8 = mul3_6 * rnbo_cos(angle6), expr_9 = mul3_6 * rnbo_sin(angle6);
        number expr_10 = 0;
        number expr_11 = 0;
        expr_10 = mul4_7 * rnbo_cos(angle8), expr_11 = mul4_7 * rnbo_sin(angle8);
        out6[(Index)i] = expr_9;
        out3[(Index)i] = expr_6;
        out7[(Index)i] = expr_10;
        out4[(Index)i] = expr_7;
        out8[(Index)i] = expr_11;
        out1[(Index)i] = expr_4;
        out2[(Index)i] = expr_5;
        out5[(Index)i] = expr_8;
        this->gen_01_del3_step();
        this->gen_01_del2_step();
        this->gen_01_del1_step();
    }
}

void ifftstream_tilde_01_perform(
    const Sample * realIn,
    const Sample * imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    auto __ifftstream_tilde_01_datapos = this->ifftstream_tilde_01_datapos;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        number invFftSize = (number)1 / (number)1024;
        SampleIndex i = (SampleIndex)(__ifftstream_tilde_01_datapos);
        list out = list(0, 0, i);
        number windowFactor = 1;

        if (__ifftstream_tilde_01_datapos < 1024) {
            this->ifftstream_tilde_01_inWorkspace[(Index)(2 * i + 1)] = realIn[(Index)i0];
            out[0] = windowFactor * this->ifftstream_tilde_01_outWorkspace[(Index)(2 * i + 1)];
            this->ifftstream_tilde_01_inWorkspace[(Index)(2 * i)] = imagIn[(Index)i0];
            out[1] = windowFactor * this->ifftstream_tilde_01_outWorkspace[(Index)(2 * i)];

            if (i == 1024 - 1) {
                this->ifftstream_tilde_01_fft_next(this->ifftstream_tilde_01_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->ifftstream_tilde_01_outWorkspace[(Index)j] = this->ifftstream_tilde_01_inWorkspace[(Index)j] * invFftSize;
                }
            }

            __ifftstream_tilde_01_datapos = (__ifftstream_tilde_01_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->ifftstream_tilde_01_datapos = __ifftstream_tilde_01_datapos;
}

void dspexpr_01_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.5;//#map:_###_obj_###_:1
    }
}

void ifftstream_tilde_02_perform(
    const Sample * realIn,
    const Sample * imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    auto __ifftstream_tilde_02_datapos = this->ifftstream_tilde_02_datapos;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        number invFftSize = (number)1 / (number)1024;
        SampleIndex i = (SampleIndex)(__ifftstream_tilde_02_datapos);
        list out = list(0, 0, i);
        number windowFactor = 1;

        if (__ifftstream_tilde_02_datapos < 1024) {
            this->ifftstream_tilde_02_inWorkspace[(Index)(2 * i + 1)] = realIn[(Index)i0];
            out[0] = windowFactor * this->ifftstream_tilde_02_outWorkspace[(Index)(2 * i + 1)];
            this->ifftstream_tilde_02_inWorkspace[(Index)(2 * i)] = imagIn[(Index)i0];
            out[1] = windowFactor * this->ifftstream_tilde_02_outWorkspace[(Index)(2 * i)];

            if (i == 1024 - 1) {
                this->ifftstream_tilde_02_fft_next(this->ifftstream_tilde_02_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->ifftstream_tilde_02_outWorkspace[(Index)j] = this->ifftstream_tilde_02_inWorkspace[(Index)j] * invFftSize;
                }
            }

            __ifftstream_tilde_02_datapos = (__ifftstream_tilde_02_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->ifftstream_tilde_02_datapos = __ifftstream_tilde_02_datapos;
}

void dspexpr_03_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.5;//#map:_###_obj_###_:1
    }
}

void dspexpr_02_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] + in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void ifftstream_tilde_03_perform(
    const Sample * realIn,
    const Sample * imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    auto __ifftstream_tilde_03_datapos = this->ifftstream_tilde_03_datapos;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        number invFftSize = (number)1 / (number)1024;
        SampleIndex i = (SampleIndex)(__ifftstream_tilde_03_datapos);
        list out = list(0, 0, i);
        number windowFactor = 1;

        if (__ifftstream_tilde_03_datapos < 1024) {
            this->ifftstream_tilde_03_inWorkspace[(Index)(2 * i + 1)] = realIn[(Index)i0];
            out[0] = windowFactor * this->ifftstream_tilde_03_outWorkspace[(Index)(2 * i + 1)];
            this->ifftstream_tilde_03_inWorkspace[(Index)(2 * i)] = imagIn[(Index)i0];
            out[1] = windowFactor * this->ifftstream_tilde_03_outWorkspace[(Index)(2 * i)];

            if (i == 1024 - 1) {
                this->ifftstream_tilde_03_fft_next(this->ifftstream_tilde_03_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->ifftstream_tilde_03_outWorkspace[(Index)j] = this->ifftstream_tilde_03_inWorkspace[(Index)j] * invFftSize;
                }
            }

            __ifftstream_tilde_03_datapos = (__ifftstream_tilde_03_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->ifftstream_tilde_03_datapos = __ifftstream_tilde_03_datapos;
}

void dspexpr_04_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.5;//#map:_###_obj_###_:1
    }
}

void ifftstream_tilde_04_perform(
    const Sample * realIn,
    const Sample * imagIn,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    Index n
) {
    auto __ifftstream_tilde_04_datapos = this->ifftstream_tilde_04_datapos;
    Index i0;

    for (i0 = 0; i0 < n; i0++) {
        number invFftSize = (number)1 / (number)1024;
        SampleIndex i = (SampleIndex)(__ifftstream_tilde_04_datapos);
        list out = list(0, 0, i);
        number windowFactor = 1;

        if (__ifftstream_tilde_04_datapos < 1024) {
            this->ifftstream_tilde_04_inWorkspace[(Index)(2 * i + 1)] = realIn[(Index)i0];
            out[0] = windowFactor * this->ifftstream_tilde_04_outWorkspace[(Index)(2 * i + 1)];
            this->ifftstream_tilde_04_inWorkspace[(Index)(2 * i)] = imagIn[(Index)i0];
            out[1] = windowFactor * this->ifftstream_tilde_04_outWorkspace[(Index)(2 * i)];

            if (i == 1024 - 1) {
                this->ifftstream_tilde_04_fft_next(this->ifftstream_tilde_04_inWorkspace, 1024);

                for (SampleIndex j = 0; j < 1024 * 2; j++) {
                    this->ifftstream_tilde_04_outWorkspace[(Index)j] = this->ifftstream_tilde_04_inWorkspace[(Index)j] * invFftSize;
                }
            }

            __ifftstream_tilde_04_datapos = (__ifftstream_tilde_04_datapos + 1) % 1024;
        }

        out1[(Index)i0] = out[0];
        out2[(Index)i0] = out[1];
        out3[(Index)i0] = out[2];
    }

    this->ifftstream_tilde_04_datapos = __ifftstream_tilde_04_datapos;
}

void dspexpr_06_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.5;//#map:_###_obj_###_:1
    }
}

void dspexpr_05_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] + in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void gen_01_del3_step() {
    this->gen_01_del3_reader++;

    if (this->gen_01_del3_reader >= (int)(this->gen_01_del3_buffer->getSize()))
        this->gen_01_del3_reader = 0;
}

number gen_01_del3_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del3_buffer->getSize()) + this->gen_01_del3_reader - ((size > this->gen_01_del3__maxdelay ? this->gen_01_del3__maxdelay : (size < (this->gen_01_del3_reader != this->gen_01_del3_writer) ? this->gen_01_del3_reader != this->gen_01_del3_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del3_wrap))
        ), this->gen_01_del3_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del3_wrap))
        ));
    }

    number r = (int)(this->gen_01_del3_buffer->getSize()) + this->gen_01_del3_reader - ((size > this->gen_01_del3__maxdelay ? this->gen_01_del3__maxdelay : (size < (this->gen_01_del3_reader != this->gen_01_del3_writer) ? this->gen_01_del3_reader != this->gen_01_del3_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del3_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del3_wrap))
    );
}

void gen_01_del3_write(number v) {
    this->gen_01_del3_writer = this->gen_01_del3_reader;
    this->gen_01_del3_buffer[(Index)this->gen_01_del3_writer] = v;
}

number gen_01_del3_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del3__maxdelay : size);
    number val = this->gen_01_del3_read(effectiveSize, 0);
    this->gen_01_del3_write(v);
    this->gen_01_del3_step();
    return val;
}

array<Index, 2> gen_01_del3_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del3_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del3_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del3_init() {
    auto result = this->gen_01_del3_calcSizeInSamples();
    this->gen_01_del3__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del3_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del3_wrap = requestedSizeInSamples - 1;
}

void gen_01_del3_clear() {
    this->gen_01_del3_buffer->setZero();
}

void gen_01_del3_reset() {
    auto result = this->gen_01_del3_calcSizeInSamples();
    this->gen_01_del3__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del3_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del3_buffer);
    this->gen_01_del3_wrap = this->gen_01_del3_buffer->getSize() - 1;
    this->gen_01_del3_clear();

    if (this->gen_01_del3_reader >= this->gen_01_del3__maxdelay || this->gen_01_del3_writer >= this->gen_01_del3__maxdelay) {
        this->gen_01_del3_reader = 0;
        this->gen_01_del3_writer = 0;
    }
}

void gen_01_del3_dspsetup() {
    this->gen_01_del3_reset();
}

number gen_01_del3_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del3_size() {
    return this->gen_01_del3__maxdelay;
}

void gen_01_del2_step() {
    this->gen_01_del2_reader++;

    if (this->gen_01_del2_reader >= (int)(this->gen_01_del2_buffer->getSize()))
        this->gen_01_del2_reader = 0;
}

number gen_01_del2_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del2_buffer->getSize()) + this->gen_01_del2_reader - ((size > this->gen_01_del2__maxdelay ? this->gen_01_del2__maxdelay : (size < (this->gen_01_del2_reader != this->gen_01_del2_writer) ? this->gen_01_del2_reader != this->gen_01_del2_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del2_wrap))
        ), this->gen_01_del2_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del2_wrap))
        ));
    }

    number r = (int)(this->gen_01_del2_buffer->getSize()) + this->gen_01_del2_reader - ((size > this->gen_01_del2__maxdelay ? this->gen_01_del2__maxdelay : (size < (this->gen_01_del2_reader != this->gen_01_del2_writer) ? this->gen_01_del2_reader != this->gen_01_del2_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del2_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del2_wrap))
    );
}

void gen_01_del2_write(number v) {
    this->gen_01_del2_writer = this->gen_01_del2_reader;
    this->gen_01_del2_buffer[(Index)this->gen_01_del2_writer] = v;
}

number gen_01_del2_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del2__maxdelay : size);
    number val = this->gen_01_del2_read(effectiveSize, 0);
    this->gen_01_del2_write(v);
    this->gen_01_del2_step();
    return val;
}

array<Index, 2> gen_01_del2_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del2_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del2_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del2_init() {
    auto result = this->gen_01_del2_calcSizeInSamples();
    this->gen_01_del2__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del2_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del2_wrap = requestedSizeInSamples - 1;
}

void gen_01_del2_clear() {
    this->gen_01_del2_buffer->setZero();
}

void gen_01_del2_reset() {
    auto result = this->gen_01_del2_calcSizeInSamples();
    this->gen_01_del2__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del2_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del2_buffer);
    this->gen_01_del2_wrap = this->gen_01_del2_buffer->getSize() - 1;
    this->gen_01_del2_clear();

    if (this->gen_01_del2_reader >= this->gen_01_del2__maxdelay || this->gen_01_del2_writer >= this->gen_01_del2__maxdelay) {
        this->gen_01_del2_reader = 0;
        this->gen_01_del2_writer = 0;
    }
}

void gen_01_del2_dspsetup() {
    this->gen_01_del2_reset();
}

number gen_01_del2_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del2_size() {
    return this->gen_01_del2__maxdelay;
}

void gen_01_del1_step() {
    this->gen_01_del1_reader++;

    if (this->gen_01_del1_reader >= (int)(this->gen_01_del1_buffer->getSize()))
        this->gen_01_del1_reader = 0;
}

number gen_01_del1_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (this->gen_01_del1_reader != this->gen_01_del1_writer) ? this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
        long index1 = (long)(rnbo_floor(r));
        number frac = r - index1;
        long index2 = (long)(index1 + 1);

        return this->linearinterp(frac, this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
        ), this->gen_01_del1_buffer->getSample(
            0,
            (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_del1_wrap))
        ));
    }

    number r = (int)(this->gen_01_del1_buffer->getSize()) + this->gen_01_del1_reader - ((size > this->gen_01_del1__maxdelay ? this->gen_01_del1__maxdelay : (size < (this->gen_01_del1_reader != this->gen_01_del1_writer) ? this->gen_01_del1_reader != this->gen_01_del1_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_del1_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_del1_wrap))
    );
}

void gen_01_del1_write(number v) {
    this->gen_01_del1_writer = this->gen_01_del1_reader;
    this->gen_01_del1_buffer[(Index)this->gen_01_del1_writer] = v;
}

number gen_01_del1_next(number v, int size) {
    number effectiveSize = (size == -1 ? this->gen_01_del1__maxdelay : size);
    number val = this->gen_01_del1_read(effectiveSize, 0);
    this->gen_01_del1_write(v);
    this->gen_01_del1_step();
    return val;
}

array<Index, 2> gen_01_del1_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_del1_evaluateSizeExpr(this->samplerate(), this->vectorsize());
        this->gen_01_del1_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_del1_init() {
    auto result = this->gen_01_del1_calcSizeInSamples();
    this->gen_01_del1__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_del1_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_del1_wrap = requestedSizeInSamples - 1;
}

void gen_01_del1_clear() {
    this->gen_01_del1_buffer->setZero();
}

void gen_01_del1_reset() {
    auto result = this->gen_01_del1_calcSizeInSamples();
    this->gen_01_del1__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_del1_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_del1_buffer);
    this->gen_01_del1_wrap = this->gen_01_del1_buffer->getSize() - 1;
    this->gen_01_del1_clear();

    if (this->gen_01_del1_reader >= this->gen_01_del1__maxdelay || this->gen_01_del1_writer >= this->gen_01_del1__maxdelay) {
        this->gen_01_del1_reader = 0;
        this->gen_01_del1_writer = 0;
    }
}

void gen_01_del1_dspsetup() {
    this->gen_01_del1_reset();
}

number gen_01_del1_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->samplerate();
}

number gen_01_del1_size() {
    return this->gen_01_del1__maxdelay;
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_del3_dspsetup();
    this->gen_01_del2_dspsetup();
    this->gen_01_del1_dspsetup();
}

template <typename T> void fftstream_tilde_01_fft_next(T& buffer, int fftsize) {
    if (this->fftstream_tilde_01_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->fftstream_tilde_01_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->fftstream_tilde_01_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->fftstream_tilde_01_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->fftstream_tilde_01_fft_lastsize = fftsize;
    }

    if (this->fftstream_tilde_01_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->fftstream_tilde_01_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->fftstream_tilde_01_fft_costab[(Index)k] + imag_l * this->fftstream_tilde_01_fft_sintab[(Index)k];
                    number tpim = -real_l * this->fftstream_tilde_01_fft_sintab[(Index)k] + imag_l * this->fftstream_tilde_01_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void fftstream_tilde_01_fft_reset() {
    this->fftstream_tilde_01_fft_lastsize = 0;
    this->fftstream_tilde_01_fft_costab = {};
    this->fftstream_tilde_01_fft_sintab = {};
    this->fftstream_tilde_01_fft_levels = -1;
}

void fftstream_tilde_01_dspsetup(bool force) {
    if ((bool)(this->fftstream_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->fftstream_tilde_01_inWorkspace[(Index)i] = 0;
        this->fftstream_tilde_01_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(0 % safeframesize);

    if (framepos < 1024) {
        this->fftstream_tilde_01_datapos = framepos;
    } else {
        this->fftstream_tilde_01_datapos = 0;
    }

    this->fftstream_tilde_01_setupDone = true;
}

template <typename T> void ifftstream_tilde_01_fft_next(T& buffer, int fftsize) {
    if (this->ifftstream_tilde_01_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->ifftstream_tilde_01_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->ifftstream_tilde_01_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->ifftstream_tilde_01_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->ifftstream_tilde_01_fft_lastsize = fftsize;
    }

    if (this->ifftstream_tilde_01_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->ifftstream_tilde_01_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->ifftstream_tilde_01_fft_costab[(Index)k] + imag_l * this->ifftstream_tilde_01_fft_sintab[(Index)k];
                    number tpim = -real_l * this->ifftstream_tilde_01_fft_sintab[(Index)k] + imag_l * this->ifftstream_tilde_01_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void ifftstream_tilde_01_fft_reset() {
    this->ifftstream_tilde_01_fft_lastsize = 0;
    this->ifftstream_tilde_01_fft_costab = {};
    this->ifftstream_tilde_01_fft_sintab = {};
    this->ifftstream_tilde_01_fft_levels = -1;
}

void ifftstream_tilde_01_dspsetup(bool force) {
    if ((bool)(this->ifftstream_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->ifftstream_tilde_01_inWorkspace[(Index)i] = 0;
        this->ifftstream_tilde_01_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(0 % safeframesize);

    if (framepos < 1024) {
        this->ifftstream_tilde_01_datapos = framepos;
    } else {
        this->ifftstream_tilde_01_datapos = 0;
    }

    this->ifftstream_tilde_01_setupDone = true;
}

template <typename T> void fftstream_tilde_02_fft_next(T& buffer, int fftsize) {
    if (this->fftstream_tilde_02_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->fftstream_tilde_02_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->fftstream_tilde_02_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->fftstream_tilde_02_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->fftstream_tilde_02_fft_lastsize = fftsize;
    }

    if (this->fftstream_tilde_02_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->fftstream_tilde_02_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->fftstream_tilde_02_fft_costab[(Index)k] + imag_l * this->fftstream_tilde_02_fft_sintab[(Index)k];
                    number tpim = -real_l * this->fftstream_tilde_02_fft_sintab[(Index)k] + imag_l * this->fftstream_tilde_02_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void fftstream_tilde_02_fft_reset() {
    this->fftstream_tilde_02_fft_lastsize = 0;
    this->fftstream_tilde_02_fft_costab = {};
    this->fftstream_tilde_02_fft_sintab = {};
    this->fftstream_tilde_02_fft_levels = -1;
}

void fftstream_tilde_02_dspsetup(bool force) {
    if ((bool)(this->fftstream_tilde_02_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->fftstream_tilde_02_inWorkspace[(Index)i] = 0;
        this->fftstream_tilde_02_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(512 % safeframesize);

    if (framepos < 1024) {
        this->fftstream_tilde_02_datapos = framepos;
    } else {
        this->fftstream_tilde_02_datapos = 0;
    }

    this->fftstream_tilde_02_setupDone = true;
}

template <typename T> void ifftstream_tilde_02_fft_next(T& buffer, int fftsize) {
    if (this->ifftstream_tilde_02_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->ifftstream_tilde_02_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->ifftstream_tilde_02_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->ifftstream_tilde_02_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->ifftstream_tilde_02_fft_lastsize = fftsize;
    }

    if (this->ifftstream_tilde_02_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->ifftstream_tilde_02_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->ifftstream_tilde_02_fft_costab[(Index)k] + imag_l * this->ifftstream_tilde_02_fft_sintab[(Index)k];
                    number tpim = -real_l * this->ifftstream_tilde_02_fft_sintab[(Index)k] + imag_l * this->ifftstream_tilde_02_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void ifftstream_tilde_02_fft_reset() {
    this->ifftstream_tilde_02_fft_lastsize = 0;
    this->ifftstream_tilde_02_fft_costab = {};
    this->ifftstream_tilde_02_fft_sintab = {};
    this->ifftstream_tilde_02_fft_levels = -1;
}

void ifftstream_tilde_02_dspsetup(bool force) {
    if ((bool)(this->ifftstream_tilde_02_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->ifftstream_tilde_02_inWorkspace[(Index)i] = 0;
        this->ifftstream_tilde_02_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(512 % safeframesize);

    if (framepos < 1024) {
        this->ifftstream_tilde_02_datapos = framepos;
    } else {
        this->ifftstream_tilde_02_datapos = 0;
    }

    this->ifftstream_tilde_02_setupDone = true;
}

template <typename T> void fftstream_tilde_03_fft_next(T& buffer, int fftsize) {
    if (this->fftstream_tilde_03_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->fftstream_tilde_03_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->fftstream_tilde_03_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->fftstream_tilde_03_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->fftstream_tilde_03_fft_lastsize = fftsize;
    }

    if (this->fftstream_tilde_03_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->fftstream_tilde_03_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->fftstream_tilde_03_fft_costab[(Index)k] + imag_l * this->fftstream_tilde_03_fft_sintab[(Index)k];
                    number tpim = -real_l * this->fftstream_tilde_03_fft_sintab[(Index)k] + imag_l * this->fftstream_tilde_03_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void fftstream_tilde_03_fft_reset() {
    this->fftstream_tilde_03_fft_lastsize = 0;
    this->fftstream_tilde_03_fft_costab = {};
    this->fftstream_tilde_03_fft_sintab = {};
    this->fftstream_tilde_03_fft_levels = -1;
}

void fftstream_tilde_03_dspsetup(bool force) {
    if ((bool)(this->fftstream_tilde_03_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->fftstream_tilde_03_inWorkspace[(Index)i] = 0;
        this->fftstream_tilde_03_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(0 % safeframesize);

    if (framepos < 1024) {
        this->fftstream_tilde_03_datapos = framepos;
    } else {
        this->fftstream_tilde_03_datapos = 0;
    }

    this->fftstream_tilde_03_setupDone = true;
}

template <typename T> void ifftstream_tilde_03_fft_next(T& buffer, int fftsize) {
    if (this->ifftstream_tilde_03_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->ifftstream_tilde_03_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->ifftstream_tilde_03_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->ifftstream_tilde_03_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->ifftstream_tilde_03_fft_lastsize = fftsize;
    }

    if (this->ifftstream_tilde_03_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->ifftstream_tilde_03_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->ifftstream_tilde_03_fft_costab[(Index)k] + imag_l * this->ifftstream_tilde_03_fft_sintab[(Index)k];
                    number tpim = -real_l * this->ifftstream_tilde_03_fft_sintab[(Index)k] + imag_l * this->ifftstream_tilde_03_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void ifftstream_tilde_03_fft_reset() {
    this->ifftstream_tilde_03_fft_lastsize = 0;
    this->ifftstream_tilde_03_fft_costab = {};
    this->ifftstream_tilde_03_fft_sintab = {};
    this->ifftstream_tilde_03_fft_levels = -1;
}

void ifftstream_tilde_03_dspsetup(bool force) {
    if ((bool)(this->ifftstream_tilde_03_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->ifftstream_tilde_03_inWorkspace[(Index)i] = 0;
        this->ifftstream_tilde_03_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(0 % safeframesize);

    if (framepos < 1024) {
        this->ifftstream_tilde_03_datapos = framepos;
    } else {
        this->ifftstream_tilde_03_datapos = 0;
    }

    this->ifftstream_tilde_03_setupDone = true;
}

template <typename T> void fftstream_tilde_04_fft_next(T& buffer, int fftsize) {
    if (this->fftstream_tilde_04_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->fftstream_tilde_04_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->fftstream_tilde_04_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->fftstream_tilde_04_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->fftstream_tilde_04_fft_lastsize = fftsize;
    }

    if (this->fftstream_tilde_04_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->fftstream_tilde_04_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->fftstream_tilde_04_fft_costab[(Index)k] + imag_l * this->fftstream_tilde_04_fft_sintab[(Index)k];
                    number tpim = -real_l * this->fftstream_tilde_04_fft_sintab[(Index)k] + imag_l * this->fftstream_tilde_04_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void fftstream_tilde_04_fft_reset() {
    this->fftstream_tilde_04_fft_lastsize = 0;
    this->fftstream_tilde_04_fft_costab = {};
    this->fftstream_tilde_04_fft_sintab = {};
    this->fftstream_tilde_04_fft_levels = -1;
}

void fftstream_tilde_04_dspsetup(bool force) {
    if ((bool)(this->fftstream_tilde_04_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->fftstream_tilde_04_inWorkspace[(Index)i] = 0;
        this->fftstream_tilde_04_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(512 % safeframesize);

    if (framepos < 1024) {
        this->fftstream_tilde_04_datapos = framepos;
    } else {
        this->fftstream_tilde_04_datapos = 0;
    }

    this->fftstream_tilde_04_setupDone = true;
}

template <typename T> void ifftstream_tilde_04_fft_next(T& buffer, int fftsize) {
    if (this->ifftstream_tilde_04_fft_lastsize != fftsize) {
        for (Index i = 0; i < 32; i++) {
            if ((BinOpInt)((BinOpInt)1 << imod_nocast((UBinOpInt)i, 32)) == fftsize) {
                this->ifftstream_tilde_04_fft_levels = i;
            }
        }

        for (Index i = 0; i < fftsize; i++) {
            this->ifftstream_tilde_04_fft_costab->push(rnbo_cos(6.28318530717958647692 * i / fftsize));
            this->ifftstream_tilde_04_fft_sintab->push(rnbo_sin(6.28318530717958647692 * i / fftsize));
        }

        this->ifftstream_tilde_04_fft_lastsize = fftsize;
    }

    if (this->ifftstream_tilde_04_fft_levels != -1) {
        for (Index i = 0; i < fftsize; i++) {
            Index x = (Index)(i);
            Index j = 0;

            for (Index k = 0; k < this->ifftstream_tilde_04_fft_levels; k++) {
                j = (BinOpInt)((BinOpInt)((BinOpInt)j << imod_nocast((UBinOpInt)1, 32)) | (BinOpInt)((BinOpInt)x & (BinOpInt)1));
                x = (BinOpInt)((BinOpInt)x >> imod_nocast((UBinOpInt)1, 32));
            }

            if (j > i) {
                number temp = buffer[(Index)(2 * i)];
                buffer[(Index)(2 * i)] = buffer[(Index)(2 * j)];
                buffer[(Index)(2 * j)] = temp;
                temp = buffer[(Index)(2 * i + 1)];
                buffer[(Index)(2 * i + 1)] = buffer[(Index)(2 * j + 1)];
                buffer[(Index)(2 * j + 1)] = temp;
            }
        }

        for (Index size = 2; size <= fftsize; size *= 2) {
            Index halfsize = (Index)(size / (number)2);
            Index tablestep = (Index)(fftsize / size);

            for (Index i = 0; i < fftsize; i += size) {
                Index k = 0;

                for (Index j = (Index)(i); j < i + halfsize; j++) {
                    Index l = (Index)(j + halfsize);
                    number real_l = buffer[(Index)(2 * l)];
                    number imag_l = buffer[(Index)(2 * l + 1)];
                    number tpre = real_l * this->ifftstream_tilde_04_fft_costab[(Index)k] + imag_l * this->ifftstream_tilde_04_fft_sintab[(Index)k];
                    number tpim = -real_l * this->ifftstream_tilde_04_fft_sintab[(Index)k] + imag_l * this->ifftstream_tilde_04_fft_costab[(Index)k];
                    number real_j = buffer[(Index)(2 * j)];
                    number imag_j = buffer[(Index)(2 * j + 1)];
                    buffer[(Index)(2 * l)] = real_j - tpre;
                    buffer[(Index)(2 * l + 1)] = imag_j - tpim;
                    buffer[(Index)(2 * j)] = real_j + tpre;
                    buffer[(Index)(2 * j + 1)] = imag_j + tpim;
                    k += tablestep;
                }
            }
        }
    }
}

void ifftstream_tilde_04_fft_reset() {
    this->ifftstream_tilde_04_fft_lastsize = 0;
    this->ifftstream_tilde_04_fft_costab = {};
    this->ifftstream_tilde_04_fft_sintab = {};
    this->ifftstream_tilde_04_fft_levels = -1;
}

void ifftstream_tilde_04_dspsetup(bool force) {
    if ((bool)(this->ifftstream_tilde_04_setupDone) && (bool)(!(bool)(force)))
        return;

    for (int i = 0; i < 1024 * 2; i++) {
        this->ifftstream_tilde_04_inWorkspace[(Index)i] = 0;
        this->ifftstream_tilde_04_outWorkspace[(Index)i] = 0;
    }

    int safeframesize = (int)(1024);

    {
        safeframesize = nextpoweroftwo(1024);
    }

    int framepos = (int)(512 % safeframesize);

    if (framepos < 1024) {
        this->ifftstream_tilde_04_datapos = framepos;
    } else {
        this->ifftstream_tilde_04_datapos = 0;
    }

    this->ifftstream_tilde_04_setupDone = true;
}

number gen_02_phasor_7_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_02_phasor_7_conv;

    if (this->gen_02_phasor_7_currentPhase < 0.)
        this->gen_02_phasor_7_currentPhase = 1. + this->gen_02_phasor_7_currentPhase;

    if (this->gen_02_phasor_7_currentPhase > 1.)
        this->gen_02_phasor_7_currentPhase = this->gen_02_phasor_7_currentPhase - 1.;

    number tmp = this->gen_02_phasor_7_currentPhase;
    this->gen_02_phasor_7_currentPhase += pincr;
    return tmp;
}

void gen_02_phasor_7_reset() {
    this->gen_02_phasor_7_currentPhase = 0;
}

void gen_02_phasor_7_dspsetup() {
    this->gen_02_phasor_7_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

void gen_02_dspsetup(bool force) {
    if ((bool)(this->gen_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_02_setupDone = true;
    this->gen_02_phasor_7_dspsetup();
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

Index globaltransport_getSampleOffset(MillisecondTime time) {
    return this->mstosamps(this->maximum(0, time - this->getEngine()->getCurrentTime()));
}

number globaltransport_getTempoAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_tempo[(Index)sampleOffset] : this->globaltransport_lastTempo);
}

number globaltransport_getStateAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_state[(Index)sampleOffset] : this->globaltransport_lastState);
}

number globaltransport_getState(MillisecondTime time) {
    return this->globaltransport_getStateAtSample(this->globaltransport_getSampleOffset(time));
}

number globaltransport_getBeatTime(MillisecondTime time) {
    number i = 2;

    while (i < this->globaltransport_beatTimeChanges->length && this->globaltransport_beatTimeChanges[(Index)(i + 1)] <= time) {
        i += 2;
    }

    i -= 2;
    number beatTimeBase = this->globaltransport_beatTimeChanges[(Index)i];

    if (this->globaltransport_getState(time) == 0)
        return beatTimeBase;

    number beatTimeBaseMsTime = this->globaltransport_beatTimeChanges[(Index)(i + 1)];
    number diff = time - beatTimeBaseMsTime;
    return beatTimeBase + this->mstobeats(diff);
}

bool globaltransport_setTempo(MillisecondTime time, number tempo, bool notify) {
    if ((bool)(notify)) {
        this->processTempoEvent(time, tempo);
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getTempoAtSample(offset) != tempo) {
            this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
            this->globaltransport_beatTimeChanges->push(time);
            fillSignal(this->globaltransport_tempo, this->vs, tempo, offset);
            this->globaltransport_lastTempo = tempo;
            this->globaltransport_tempoNeedsReset = true;
            return true;
        }
    }

    return false;
}

number globaltransport_getTempo(MillisecondTime time) {
    return this->globaltransport_getTempoAtSample(this->globaltransport_getSampleOffset(time));
}

bool globaltransport_setState(MillisecondTime time, number state, bool notify) {
    if ((bool)(notify)) {
        this->processTransportEvent(time, TransportState(state));
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getStateAtSample(offset) != state) {
            fillSignal(this->globaltransport_state, this->vs, state, offset);
            this->globaltransport_lastState = TransportState(state);
            this->globaltransport_stateNeedsReset = true;

            if (state == 0) {
                this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
                this->globaltransport_beatTimeChanges->push(time);
            }

            return true;
        }
    }

    return false;
}

bool globaltransport_setBeatTime(MillisecondTime time, number beattime, bool notify) {
    if ((bool)(notify)) {
        this->processBeatTimeEvent(time, beattime);
        this->globaltransport_notify = true;
        return false;
    } else {
        bool beatTimeHasChanged = false;
        float oldBeatTime = (float)(this->globaltransport_getBeatTime(time));
        float newBeatTime = (float)(beattime);

        if (oldBeatTime != newBeatTime) {
            beatTimeHasChanged = true;
        }

        this->globaltransport_beatTimeChanges->push(beattime);
        this->globaltransport_beatTimeChanges->push(time);
        return beatTimeHasChanged;
    }
}

number globaltransport_getBeatTimeAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getBeatTime(this->getEngine()->getCurrentTime() + msOffset);
}

array<number, 2> globaltransport_getTimeSignature(MillisecondTime time) {
    number i = 3;

    while (i < this->globaltransport_timeSignatureChanges->length && this->globaltransport_timeSignatureChanges[(Index)(i + 2)] <= time) {
        i += 3;
    }

    i -= 3;

    return {
        this->globaltransport_timeSignatureChanges[(Index)i],
        this->globaltransport_timeSignatureChanges[(Index)(i + 1)]
    };
}

array<number, 2> globaltransport_getTimeSignatureAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getTimeSignature(this->getEngine()->getCurrentTime() + msOffset);
}

bool globaltransport_setTimeSignature(MillisecondTime time, number numerator, number denominator, bool notify) {
    if ((bool)(notify)) {
        this->processTimeSignatureEvent(time, (int)(numerator), (int)(denominator));
        this->globaltransport_notify = true;
    } else {
        array<number, 2> currentSig = this->globaltransport_getTimeSignature(time);

        if (currentSig[0] != numerator || currentSig[1] != denominator) {
            this->globaltransport_timeSignatureChanges->push(numerator);
            this->globaltransport_timeSignatureChanges->push(denominator);
            this->globaltransport_timeSignatureChanges->push(time);
            return true;
        }
    }

    return false;
}

void globaltransport_advance() {
    if ((bool)(this->globaltransport_tempoNeedsReset)) {
        fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
        this->globaltransport_tempoNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTempoEvent(this->globaltransport_lastTempo);
        }
    }

    if ((bool)(this->globaltransport_stateNeedsReset)) {
        fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
        this->globaltransport_stateNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTransportEvent(TransportState(this->globaltransport_lastState));
        }
    }

    if (this->globaltransport_beatTimeChanges->length > 2) {
        this->globaltransport_beatTimeChanges[0] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 2)];
        this->globaltransport_beatTimeChanges[1] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 1)];
        this->globaltransport_beatTimeChanges->length = 2;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendBeatTimeEvent(this->globaltransport_beatTimeChanges[0]);
        }
    }

    if (this->globaltransport_timeSignatureChanges->length > 3) {
        this->globaltransport_timeSignatureChanges[0] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 3)];
        this->globaltransport_timeSignatureChanges[1] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 2)];
        this->globaltransport_timeSignatureChanges[2] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 1)];
        this->globaltransport_timeSignatureChanges->length = 3;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTimeSignatureEvent(
                (int)(this->globaltransport_timeSignatureChanges[0]),
                (int)(this->globaltransport_timeSignatureChanges[1])
            );
        }
    }

    this->globaltransport_notify = false;
}

void globaltransport_dspsetup(bool force) {
    if ((bool)(this->globaltransport_setupDone) && (bool)(!(bool)(force)))
        return;

    fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
    this->globaltransport_tempoNeedsReset = false;
    fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
    this->globaltransport_stateNeedsReset = false;
    this->globaltransport_setupDone = true;
}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    gen_01_in1 = 0;
    gen_01_in2 = 0;
    gen_01_in3 = 0;
    gen_01_in4 = 0;
    gen_01_in5 = 0;
    gen_01_in6 = 0;
    gen_01_in7 = 0;
    gen_01_in8 = 0;
    gen_01_in9 = 0;
    gen_01_in10 = 0;
    gen_01_direction = 0;
    fftstream_tilde_01_realIn = 0;
    fftstream_tilde_01_imagIn = 0;
    dspexpr_01_in1 = 0;
    dspexpr_01_in2 = 0.5;
    dspexpr_02_in1 = 0;
    dspexpr_02_in2 = 0;
    ifftstream_tilde_01_realIn = 0;
    ifftstream_tilde_01_imagIn = 0;
    fftstream_tilde_02_realIn = 0;
    fftstream_tilde_02_imagIn = 0;
    dspexpr_03_in1 = 0;
    dspexpr_03_in2 = 0.5;
    ifftstream_tilde_02_realIn = 0;
    ifftstream_tilde_02_imagIn = 0;
    fftstream_tilde_03_realIn = 0;
    fftstream_tilde_03_imagIn = 0;
    dspexpr_04_in1 = 0;
    dspexpr_04_in2 = 0.5;
    dspexpr_05_in1 = 0;
    dspexpr_05_in2 = 0;
    ifftstream_tilde_03_realIn = 0;
    ifftstream_tilde_03_imagIn = 0;
    fftstream_tilde_04_realIn = 0;
    fftstream_tilde_04_imagIn = 0;
    dspexpr_06_in1 = 0;
    dspexpr_06_in2 = 0.5;
    ifftstream_tilde_04_realIn = 0;
    ifftstream_tilde_04_imagIn = 0;
    gen_02_in1 = 0;
    gen_02_multype = 0;
    gen_02_freqOffset = 0;
    gen_02_dutyCycle = 0;
    param_01_value = 0;
    param_02_value = 0;
    param_03_value = 0;
    param_04_value = 0.5;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    signals[3] = nullptr;
    signals[4] = nullptr;
    signals[5] = nullptr;
    signals[6] = nullptr;
    signals[7] = nullptr;
    signals[8] = nullptr;
    signals[9] = nullptr;
    signals[10] = nullptr;
    signals[11] = nullptr;
    signals[12] = nullptr;
    signals[13] = nullptr;
    signals[14] = nullptr;
    signals[15] = nullptr;
    signals[16] = nullptr;
    signals[17] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    gen_01_del3__maxdelay = 0;
    gen_01_del3_sizemode = 0;
    gen_01_del3_wrap = 0;
    gen_01_del3_reader = 0;
    gen_01_del3_writer = 0;
    gen_01_del2__maxdelay = 0;
    gen_01_del2_sizemode = 0;
    gen_01_del2_wrap = 0;
    gen_01_del2_reader = 0;
    gen_01_del2_writer = 0;
    gen_01_del1__maxdelay = 0;
    gen_01_del1_sizemode = 0;
    gen_01_del1_wrap = 0;
    gen_01_del1_reader = 0;
    gen_01_del1_writer = 0;
    gen_01_setupDone = false;
    fftstream_tilde_01_datapos = 0;
    fftstream_tilde_01_fft_lastsize = 0;
    fftstream_tilde_01_fft_levels = -1;
    fftstream_tilde_01_setupDone = false;
    ifftstream_tilde_01_datapos = 0;
    ifftstream_tilde_01_fft_lastsize = 0;
    ifftstream_tilde_01_fft_levels = -1;
    ifftstream_tilde_01_setupDone = false;
    fftstream_tilde_02_datapos = 0;
    fftstream_tilde_02_fft_lastsize = 0;
    fftstream_tilde_02_fft_levels = -1;
    fftstream_tilde_02_setupDone = false;
    ifftstream_tilde_02_datapos = 0;
    ifftstream_tilde_02_fft_lastsize = 0;
    ifftstream_tilde_02_fft_levels = -1;
    ifftstream_tilde_02_setupDone = false;
    fftstream_tilde_03_datapos = 0;
    fftstream_tilde_03_fft_lastsize = 0;
    fftstream_tilde_03_fft_levels = -1;
    fftstream_tilde_03_setupDone = false;
    ifftstream_tilde_03_datapos = 0;
    ifftstream_tilde_03_fft_lastsize = 0;
    ifftstream_tilde_03_fft_levels = -1;
    ifftstream_tilde_03_setupDone = false;
    fftstream_tilde_04_datapos = 0;
    fftstream_tilde_04_fft_lastsize = 0;
    fftstream_tilde_04_fft_levels = -1;
    fftstream_tilde_04_setupDone = false;
    ifftstream_tilde_04_datapos = 0;
    ifftstream_tilde_04_fft_lastsize = 0;
    ifftstream_tilde_04_fft_levels = -1;
    ifftstream_tilde_04_setupDone = false;
    gen_02_phasor_7_currentPhase = 0;
    gen_02_phasor_7_conv = 0;
    gen_02_setupDone = false;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_tempoNeedsReset = false;
    globaltransport_lastTempo = 120;
    globaltransport_state = nullptr;
    globaltransport_stateNeedsReset = false;
    globaltransport_lastState = 0;
    globaltransport_beatTimeChanges = { 0, 0 };
    globaltransport_timeSignatureChanges = { 4, 4, 0 };
    globaltransport_notify = false;
    globaltransport_setupDone = false;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number gen_01_in1;
    number gen_01_in2;
    number gen_01_in3;
    number gen_01_in4;
    number gen_01_in5;
    number gen_01_in6;
    number gen_01_in7;
    number gen_01_in8;
    number gen_01_in9;
    number gen_01_in10;
    number gen_01_direction;
    number fftstream_tilde_01_realIn;
    number fftstream_tilde_01_imagIn;
    number dspexpr_01_in1;
    number dspexpr_01_in2;
    number dspexpr_02_in1;
    number dspexpr_02_in2;
    number ifftstream_tilde_01_realIn;
    number ifftstream_tilde_01_imagIn;
    number fftstream_tilde_02_realIn;
    number fftstream_tilde_02_imagIn;
    number dspexpr_03_in1;
    number dspexpr_03_in2;
    number ifftstream_tilde_02_realIn;
    number ifftstream_tilde_02_imagIn;
    number fftstream_tilde_03_realIn;
    number fftstream_tilde_03_imagIn;
    number dspexpr_04_in1;
    number dspexpr_04_in2;
    number dspexpr_05_in1;
    number dspexpr_05_in2;
    number ifftstream_tilde_03_realIn;
    number ifftstream_tilde_03_imagIn;
    number fftstream_tilde_04_realIn;
    number fftstream_tilde_04_imagIn;
    number dspexpr_06_in1;
    number dspexpr_06_in2;
    number ifftstream_tilde_04_realIn;
    number ifftstream_tilde_04_imagIn;
    number gen_02_in1;
    number gen_02_multype;
    number gen_02_freqOffset;
    number gen_02_dutyCycle;
    number param_01_value;
    number param_02_value;
    number param_03_value;
    number param_04_value;
    MillisecondTime _currentTime;
    SampleIndex audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[18];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    Float64BufferRef gen_01_del3_buffer;
    Index gen_01_del3__maxdelay;
    Int gen_01_del3_sizemode;
    Index gen_01_del3_wrap;
    Int gen_01_del3_reader;
    Int gen_01_del3_writer;
    Float64BufferRef gen_01_del2_buffer;
    Index gen_01_del2__maxdelay;
    Int gen_01_del2_sizemode;
    Index gen_01_del2_wrap;
    Int gen_01_del2_reader;
    Int gen_01_del2_writer;
    Float64BufferRef gen_01_del1_buffer;
    Index gen_01_del1__maxdelay;
    Int gen_01_del1_sizemode;
    Index gen_01_del1_wrap;
    Int gen_01_del1_reader;
    Int gen_01_del1_writer;
    bool gen_01_setupDone;
    SampleValue fftstream_tilde_01_inWorkspace[2048] = { };
    SampleValue fftstream_tilde_01_outWorkspace[2048] = { };
    Float32BufferRef fftstream_tilde_01_win_buf;
    SampleIndex fftstream_tilde_01_datapos;
    int fftstream_tilde_01_fft_lastsize;
    list fftstream_tilde_01_fft_costab;
    list fftstream_tilde_01_fft_sintab;
    int fftstream_tilde_01_fft_levels;
    bool fftstream_tilde_01_setupDone;
    SampleValue ifftstream_tilde_01_inWorkspace[2048] = { };
    SampleValue ifftstream_tilde_01_outWorkspace[2048] = { };
    Float32BufferRef ifftstream_tilde_01_win_buf;
    SampleIndex ifftstream_tilde_01_datapos;
    int ifftstream_tilde_01_fft_lastsize;
    list ifftstream_tilde_01_fft_costab;
    list ifftstream_tilde_01_fft_sintab;
    int ifftstream_tilde_01_fft_levels;
    bool ifftstream_tilde_01_setupDone;
    SampleValue fftstream_tilde_02_inWorkspace[2048] = { };
    SampleValue fftstream_tilde_02_outWorkspace[2048] = { };
    Float32BufferRef fftstream_tilde_02_win_buf;
    SampleIndex fftstream_tilde_02_datapos;
    int fftstream_tilde_02_fft_lastsize;
    list fftstream_tilde_02_fft_costab;
    list fftstream_tilde_02_fft_sintab;
    int fftstream_tilde_02_fft_levels;
    bool fftstream_tilde_02_setupDone;
    SampleValue ifftstream_tilde_02_inWorkspace[2048] = { };
    SampleValue ifftstream_tilde_02_outWorkspace[2048] = { };
    Float32BufferRef ifftstream_tilde_02_win_buf;
    SampleIndex ifftstream_tilde_02_datapos;
    int ifftstream_tilde_02_fft_lastsize;
    list ifftstream_tilde_02_fft_costab;
    list ifftstream_tilde_02_fft_sintab;
    int ifftstream_tilde_02_fft_levels;
    bool ifftstream_tilde_02_setupDone;
    SampleValue fftstream_tilde_03_inWorkspace[2048] = { };
    SampleValue fftstream_tilde_03_outWorkspace[2048] = { };
    Float32BufferRef fftstream_tilde_03_win_buf;
    SampleIndex fftstream_tilde_03_datapos;
    int fftstream_tilde_03_fft_lastsize;
    list fftstream_tilde_03_fft_costab;
    list fftstream_tilde_03_fft_sintab;
    int fftstream_tilde_03_fft_levels;
    bool fftstream_tilde_03_setupDone;
    SampleValue ifftstream_tilde_03_inWorkspace[2048] = { };
    SampleValue ifftstream_tilde_03_outWorkspace[2048] = { };
    Float32BufferRef ifftstream_tilde_03_win_buf;
    SampleIndex ifftstream_tilde_03_datapos;
    int ifftstream_tilde_03_fft_lastsize;
    list ifftstream_tilde_03_fft_costab;
    list ifftstream_tilde_03_fft_sintab;
    int ifftstream_tilde_03_fft_levels;
    bool ifftstream_tilde_03_setupDone;
    SampleValue fftstream_tilde_04_inWorkspace[2048] = { };
    SampleValue fftstream_tilde_04_outWorkspace[2048] = { };
    Float32BufferRef fftstream_tilde_04_win_buf;
    SampleIndex fftstream_tilde_04_datapos;
    int fftstream_tilde_04_fft_lastsize;
    list fftstream_tilde_04_fft_costab;
    list fftstream_tilde_04_fft_sintab;
    int fftstream_tilde_04_fft_levels;
    bool fftstream_tilde_04_setupDone;
    SampleValue ifftstream_tilde_04_inWorkspace[2048] = { };
    SampleValue ifftstream_tilde_04_outWorkspace[2048] = { };
    Float32BufferRef ifftstream_tilde_04_win_buf;
    SampleIndex ifftstream_tilde_04_datapos;
    int ifftstream_tilde_04_fft_lastsize;
    list ifftstream_tilde_04_fft_costab;
    list ifftstream_tilde_04_fft_sintab;
    int ifftstream_tilde_04_fft_levels;
    bool ifftstream_tilde_04_setupDone;
    number gen_02_phasor_7_currentPhase;
    number gen_02_phasor_7_conv;
    bool gen_02_setupDone;
    number param_01_lastValue;
    number param_02_lastValue;
    number param_03_lastValue;
    number param_04_lastValue;
    signal globaltransport_tempo;
    bool globaltransport_tempoNeedsReset;
    number globaltransport_lastTempo;
    signal globaltransport_state;
    bool globaltransport_stateNeedsReset;
    number globaltransport_lastState;
    list globaltransport_beatTimeChanges;
    list globaltransport_timeSignatureChanges;
    bool globaltransport_notify;
    bool globaltransport_setupDone;
    number stackprotect_count;
    DataRef gen_01_del3_bufferobj;
    DataRef gen_01_del2_bufferobj;
    DataRef gen_01_del1_bufferobj;
    DataRef RNBODefaultFftWindow;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* creaternbomatic()
{
    return new rnbomatic();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return creaternbomatic;
}

} // end RNBO namespace

