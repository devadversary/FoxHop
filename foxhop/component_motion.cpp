/*2020.10.16 - AUTHOR : DevAdversary
��Ǳ׷��� ó���� ���� �����ϰ� ���Ƿ����� �ż��� ���� ����*/

#include "./include/component_motion.hpp"

/**
    @brief ������ ü�� �߰�
    @param [OUT] pPatt  ������ ���Ե� ä�α���ü
    @param [OUT] pVal   ��� ���� ���簪 (��� ���İ� ���۰�~������ ���� ����� �����)
    @param [IN]  nStart ��� ���� ���۰�
    @param [IN]  nEnd   ��� ���� ���ᰪ
*/
void AddChain(MOTION_PATTERN* pPatt, float* pVal, float nStart, float nEnd)
{
    VALUE_CHAIN vc;

    vc.pVal = pVal;
    vc.nStart = nStart;
    vc.nEnd = nEnd;
    pPatt->arrChain[pPatt->nChain] = vc; /*Ÿ�� ü��*/
    pPatt->nChain++;                     /*Ÿ�� ���� ����*/
}

ComponentMotion::ComponentMotion()
{
    memset(ChannelList, 0, sizeof(ChannelList));
    nCntUsedChannel = 0;
    nTotalPlaytime = 0;
    nRunTime = 0;
}

/**
    @brief  ��� ä�� �߰�
*/
void ComponentMotion::addChannel(MOTION_PATTERN patt)
{
    if (nCntUsedChannel == MAX_CHANNEL_CNT) return; /*ä�� ����*/
    ChannelList[nCntUsedChannel] = patt;
    nTotalPlaytime = max(nTotalPlaytime, (unsigned int)patt.MotionInfo.nDelay + patt.MotionInfo.nPitch); /* ä�� ���� ���ļ� �� ��� �ð� ���ϱ�*/
    nCntUsedChannel++;
}

/**
    @brief  ��� ä�� �̾���̱�
    @remark �̶��� �����̰��� �����ϰ�, ������ �ִ� ����ð� �ڿ� ���δ�.
*/
void ComponentMotion::appendChannel(MOTION_PATTERN patt)
{
    if (nCntUsedChannel == MAX_CHANNEL_CNT) return; /*ä�� ����*/
    patt.MotionInfo.nDelay = nTotalPlaytime; /*�����̰� ����*/
    ChannelList[nCntUsedChannel] = patt;
    nTotalPlaytime += patt.MotionInfo.nPitch; /*�� ��� �ð� �߰�*/
    nCntUsedChannel++;
}

/**
    @brief ä�� ���� ������
*/
int ComponentMotion::getChannelCnt()
{
    return nCntUsedChannel;
}

/**
    @brief ������ �ε����� ��ȯ�� ������
    @param idx ���� ���̽� �ε���
*/
float ComponentMotion::getChannelVal(int idx)
{
    if (idx >= MAX_CHANNEL_CNT) { return 0; }
    return ChannelList[idx].fCurrent;
}

/**
    @brief ä�� �� ������� ������
*/
unsigned int ComponentMotion::getPlayTime()
{
    return nTotalPlaytime;
}


/**
    @brief ��� ���� �ð��� �����Ѵ�
*/
void ComponentMotion::setRuntime(unsigned long time)
{
    nRunTime = time;
}

/**
    @brief ��� ������ ���� �����
*/
void ComponentMotion::clearChannel()
{
    memset(this, 0, sizeof(ComponentMotion));
}

/**
    @brief ���� �ð���ŭ ��ü ä�� ������Ʈ
    @return �� ������Ʈ �� ����� ������ FALSE (������Ʈ �Ϸ�), �ƴϸ� TRUE
    @remark TODO : ���� �Լ��� �ѹ� ����ص� ���� �ٸ� ä�ο� ���� ��������? ex) fCurrent �� Ȱ��
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
        nGapTime = nRunTime - pCh->MotionInfo.nDelay; /*��������ð� - ������ = �������ð�*/
        x = nGapTime / (float)pCh->MotionInfo.nPitch; /*��ġ ����*/

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
            *pVC->pVal = pVC->nStart + (pCh->fCurrent * (pVC->nEnd-pVC->nStart) ); /*Ÿ�� �� ������Ʈ*/
        }
    }
    return TRUE;
}
