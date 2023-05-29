/*2020.10.16 - AUTHOR : DevAdversary
모션그래픽 처리를 위해 간소하게 오실레이터 매서드 등을 제공*/

#include "./include/component_motion.hpp"

/**
    @brief 모션제어값 체인 추가
    @param [OUT] pPatt  정보가 기입될 채널구조체
    @param [OUT] pVal   모션 진행 현재값 (모션 공식과 시작값~끝값에 따라 모션이 진행됨)
    @param [IN]  nStart 모션 진행 시작값
    @param [IN]  nEnd   모션 진행 종료값
*/
void AddChain(MOTION_PATTERN* pPatt, float* pVal, float nStart, float nEnd)
{
    VALUE_CHAIN vc;

    vc.pVal = pVal;
    vc.nStart = nStart;
    vc.nEnd = nEnd;
    pPatt->arrChain[pPatt->nChain] = vc; /*타겟 체결*/
    pPatt->nChain++;                     /*타겟 갯수 증가*/
}

ComponentMotion::ComponentMotion()
{
    memset(ChannelList, 0, sizeof(ChannelList));
    nCntUsedChannel = 0;
    nTotalPlaytime = 0;
    nRunTime = 0;
}

/**
    @brief  모션 채널 추가
*/
void ComponentMotion::addChannel(MOTION_PATTERN patt)
{
    if (nCntUsedChannel == MAX_CHANNEL_CNT) return; /*채널 꽉참*/
    ChannelList[nCntUsedChannel] = patt;
    nTotalPlaytime = max(nTotalPlaytime, (unsigned int)patt.MotionInfo.nDelay + patt.MotionInfo.nPitch); /* 채널 전부 합쳐서 총 재생 시간 구하기*/
    nCntUsedChannel++;
}

/**
    @brief  모션 채널 이어붙이기
    @remark 이때는 딜레이값을 무시하고, 무조건 최대 재생시간 뒤에 붙인다.
*/
void ComponentMotion::appendChannel(MOTION_PATTERN patt)
{
    if (nCntUsedChannel == MAX_CHANNEL_CNT) return; /*채널 꽉참*/
    patt.MotionInfo.nDelay = nTotalPlaytime; /*딜레이값 변경*/
    ChannelList[nCntUsedChannel] = patt;
    nTotalPlaytime += patt.MotionInfo.nPitch; /*총 재생 시간 추가*/
    nCntUsedChannel++;
}

/**
    @brief 채널 갯수 얻어오기
*/
int ComponentMotion::getChannelCnt()
{
    return nCntUsedChannel;
}

/**
    @brief 선택한 인덱스의 변환값 얻어오기
    @param idx 제로 베이스 인덱스
*/
float ComponentMotion::getChannelVal(int idx)
{
    if (idx >= MAX_CHANNEL_CNT) { return 0; }
    return ChannelList[idx].fCurrent;
}

/**
    @brief 채널 총 재생길이 얻어오기
*/
unsigned int ComponentMotion::getPlayTime()
{
    return nTotalPlaytime;
}


/**
    @brief 모션 진행 시간을 변경한다
*/
void ComponentMotion::setRuntime(unsigned long time)
{
    nRunTime = time;
}

/**
    @brief 모션 정보를 전부 지운다
*/
void ComponentMotion::clearChannel()
{
    memset(this, 0, sizeof(ComponentMotion));
}

/**
    @brief 진행 시간만큼 전체 채널 업데이트
    @return 더 업데이트 할 모션이 없으면 FALSE (업데이트 완료), 아니면 TRUE
    @remark TODO : 발진 함수로 한번 계산해둔 값을 다른 채널에 재사용 가능한지? ex) fCurrent 의 활용
*/
BOOL ComponentMotion::update(unsigned long ElapsedTime)
{
    MOTION_PATTERN* pCh;
    VALUE_CHAIN* pVC;
    int i, k; 
    float x;
    unsigned long nGapTime;

    if (nRunTime > nTotalPlaytime) return FALSE;
    nRunTime += ElapsedTime;
    for (i = 0; i < nCntUsedChannel; i++) {
        pCh = &ChannelList[i];
        if (nRunTime < (unsigned int)pCh->MotionInfo.nDelay) continue;
        nGapTime = nRunTime - pCh->MotionInfo.nDelay; /*현재진행시간 - 딜레이 = 모션진행시간*/
        x = nGapTime / (float)pCh->MotionInfo.nPitch; /*피치 조절*/

        switch (pCh->MotionInfo.formular) {
        case eMotionForm::eMotion_None:      pCh->fCurrent = 1;                   break;
        case eMotionForm::eMotion_Pulse1:    pCh->fCurrent = Preset_Pulse1(x);    break;
        case eMotionForm::eMotion_Pulse2:    pCh->fCurrent = Preset_Pulse2(x);    break;
        case eMotionForm::eMotion_4Step:     pCh->fCurrent = Preset_4Step(x);     break;
        case eMotionForm::eMotion_Linear1:   pCh->fCurrent = Preset_Linear1(x);   break;
        case eMotionForm::eMotion_Linear2:   pCh->fCurrent = Preset_Linear2(x);   break;
        case eMotionForm::eMotion_x3_1:      pCh->fCurrent = Preset_x3_0to1_1(x); break;
        case eMotionForm::eMotion_x3_2:      pCh->fCurrent = Preset_x3_0to1_2(x); break;
        case eMotionForm::eMotion_x3_1to0_1: pCh->fCurrent = Preset_x3_1to0_1(x); break;
        case eMotionForm::eMotion_x3_1to0_2: pCh->fCurrent = Preset_x3_1to0_2(x); break;
        case eMotionForm::eMotion_Custom:    pCh->fCurrent = pCh->pfnForm(x);     break;
        default:                                                                  break;
        }

        for (k = 0; k < pCh->nChain; k++) {
            pVC = &pCh->arrChain[k];
            if (!pVC->pVal) continue;
            *pVC->pVal = pVC->nStart + (pCh->fCurrent * (pVC->nEnd-pVC->nStart) ); /*타겟 값 업데이트*/
        }
    }
    return TRUE;
}
