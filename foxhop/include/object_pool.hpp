#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>
#include <memory.h>

/**
    @brief  오브젝트를 미리 생성해둘 풀
    @remark 이 풀에 집어넣을 오브젝트 클래스는
    @n      pNext, pPrev, bActivate 멤버를 제공해야한다.
    @n      또한 생성자 역할을 대신할 Init 매서드도 제공해야한다.
    @n      오브젝트 클래스와의 코드 커플링은 감수해야한다.
*/
template <typename T> class ObjectPool {
private:
    T*   pInactivate;      /**< 놀고있는 오브젝트 포인터*/
    T*   pActivate;        /**< 열일중인 오브젝트 포인터*/
    T*   pObject;          /**< 풀*/
    BOOL bAssigned;        /**< 이 풀의 사용여부. (단독사용시 필요없음 이 풀의 상위 클래스가 사용)*/
    int  nAdditionalCnt;   /**< 최대용량 초과시 추가할당될 갯수 단위*/
    int  nCapacity;        /**< 풀링된 최대 오브젝트 갯수*/
    int  nUsingCnt;        /**< 현재 풀에서 사용중인 오브젝트 갯수*/
    int  nUsingMemoryByte; /**< 현재 풀의 메모리 사용량*/
    
public:
    ObjectPool();
    ~ObjectPool();
    BOOL Init(int nCnt = 250, int nAddCnt = 250);
    void Release();
    void Reset();
    BOOL isAssigned();
    void setAssigned(BOOL bFlag);
    int  getCapacity();
    int  getUsingCount();
    int  getUsingMemoryBytes();
    T*   getActiveHead();
    T*   activateObject();
    void deactivateObject(T* pObj);
};

/**
    @brief  오브젝트풀을 미리 생성하여 발급해주는 일종의 매니저
    @remark 풀이 필요하면 여기서 미리 초기화된 풀을 발급/반납한다.
    @n      템플릿 T에는 오브젝트가 들어간다.
    @n      ex) ObjectPoolList<ObjectMotionBox>
*/
template <typename T> class ObjectPoolList {

private:
    ObjectPool<T>** pPoolList;       /**< 스택으로 관리될 풀 리스트 (초기화 이래로 해제하지 않음)*/
    int            nCapacity;        /**< 전체 풀 갯수*/
    int            nUsingCnt;        /**< 현재 사용중인 풀 갯수*/
    int            nTotalPoolsSize;  /**< 현재 풀 전체의 메모리 사용량*/
    int            nTotalUsingSize;  /**< 현재 할당된 모든 오브젝트 크기 총합*/
public:
    ObjectPoolList();
    ~ObjectPoolList();
    BOOL           Init(int nPoolCnt, int nObjCnt);
    ObjectPool<T>* getPool();
    void           returnPool(ObjectPool<T>* pPool);
};

template <typename T> ObjectPool<T>::ObjectPool()
{
    pInactivate = 0;
    pActivate = 0;
    pObject = 0;
    bAssigned = FALSE;
    nAdditionalCnt = 0;
    nCapacity = 0;
    nUsingCnt = 0;
    nUsingMemoryByte = 0;
}

template <typename T> ObjectPool<T>::~ObjectPool() {}

/**
    @brief 풀을 초기화 하고 빈칸 리스트를 이어놓는다.
*/
template <typename T> BOOL ObjectPool<T>::Init(int nCnt, int nAddCnt)
{
    int i;

#if 0 /*디버그 모드 크기확인 기능 On Off*/
#if _DEBUG /*디버그 모드 실행시, 오브젝트 크기가 너무 크다 싶으면 알림을 띄움*/
#define LIMIT_SIZE (256)
    {
        static BOOL f = 0;
        if (sizeof(T) > LIMIT_SIZE && !f) {
            MessageBoxA(NULL, "Object size is a little big (ObjectPool Initialize routine)", "Info", MB_OK);
            f = TRUE;
        }
    }
#undef LIMIT_SIZE
#endif
#endif

    nUsingMemoryByte = sizeof(T) * nCnt;    /*지정 클래스 크기 x 할당갯수*/
    pObject = (T*)malloc(nUsingMemoryByte);
    if (!pObject) return FALSE;

    /*오브젝트 초기화 시작*/
    for (i = 0; i < nCnt; i++) {
        pObject[i].pPrev = &pObject[i - 1];
        pObject[i].pNext = &pObject[i + 1]; /*연결리스트 구축*/
        pObject[i].bActivate = FALSE;
    }
    pObject[0].pPrev = NULL;                /*맨 첫번째 원소는 이전노드를 비워줌*/
    pObject[i - 1].pNext = NULL;            /*맨 마지막 원소는 다음노드를 비워줌*/

    pInactivate = &pObject[0];              /*첫번째 오브젝트를 놀고있는 오브젝트로 등록*/
    pActivate = NULL;                       /*아직 활성화된 오브젝트 없음*/
    nAdditionalCnt = nAddCnt;               /*TODO : 나중에 없에도 될 것 같음*/
    nCapacity = nCnt;
    nUsingCnt = 0;
    bAssigned = FALSE;                      /*아직 사용중이지 않은 풀*/
    return TRUE;
}

/**
    @brief 풀 해제
*/
template <typename T> void ObjectPool<T>::Release()
{
    if (pObject) free((void*)pObject);
}

/**
    @brief  이미 만들어진 풀을 리셋한다
    @remark 이미 할당된 메모리는 그대로 사용하되
    @n      각 연결리스트만 재구축한다
*/
template <typename T> void ObjectPool<T>::Reset()
{
    int i;

    memset((void*)pObject, 0, nUsingMemoryByte);
    for (i = 0; i < nCapacity; i++) {
        pObject[i].pPrev = &pObject[i - 1];
        pObject[i].pNext = &pObject[i + 1]; /*연결리스트 구축*/
    }
    pObject[0].pPrev = NULL;                /*맨 첫번째 원소는 이전노드를 비워줌*/
    pObject[i - 1].pNext = NULL;            /*맨 마지막 원소는 다음노드를 비워줌*/
}

/**
    @brief  현재 풀의 사용 여부를 얻어온다
    @remark 이 매서드는 이 풀을 이용하는 상위 클래스를 위한것임
    @n      일반적으로는 사용 할 일이 없고, 사용해도 아무일이 없다.
*/
template <typename T> BOOL ObjectPool<T>::isAssigned()
{
    return bAssigned;
}

/**
    @brief  현재 풀의 사용 여부를 설정한다
    @remark 이 매서드는 이 풀을 이용하는 상위 클래스를 위한것임
    @n      이 풀이 외부에서 따로 관리되고 있다면, 건들지 말것
*/
template <typename T> void ObjectPool<T>::setAssigned(BOOL bFlag)
{
    bAssigned = bFlag;
}


/**
    @brief 현재 풀의 최대 한도를 얻어온다
*/
template <typename T> int ObjectPool<T>::getCapacity()
{
    return nCapacity;
}

/**
    @brief 현재 사용중인 오브젝트 수를 얻어온다
*/
template <typename T> int ObjectPool<T>::getUsingCount()
{
    return nUsingCnt;
}

/**
    @brief 잡아먹은 메모리를 반환한다
*/
template <typename T> int ObjectPool<T>::getUsingMemoryBytes()
{
    return nUsingMemoryByte;
}

/**
    @brief  활성화된 오브젝트의 가장 앞 노드를 가져온다
    @remark 오브젝트마다 작업 순서가 상관 없을때 일괄처리용으로 사용.
    @n      순서가 중요할 시, 따로따로 처리 해야함
*/
template <typename T> T* ObjectPool<T>::getActiveHead()
{
    return pActivate;
}

/**
    @brief  풀에서 오브젝트를 활성화 시킨다
    @remark 얻어올 오브젝트가 없으면 풀을 확장한다 (현재 realloc시 버그가 있어 확장 X)
*/
template <typename T> T* ObjectPool<T>::activateObject()
{
    T* pTmp;

    if (!pObject) return NULL;
    if (!pInactivate) { /*놀고 있는 오브젝트 없으면 풀 확장*/
#if 0 /* TODO : realloc 시 포인터가 바뀌면 모든 링크드리스트가 깨진다. */
        nUsingMemoryByte += sizeof(T) * nAdditionalCnt;
        pObject = (T*)realloc((void*)pObject, nUsingMemoryByte);
        if (!pObject) {
            MessageBoxA(NULL, "ObjectPool realloc Fail", "Fatal Error", MB_OK);
            exit(-1); /*더이상 정상적인 동작을 보장할 수 없으므로 프로그램 강제 종료*/
            return -1;
        }
        nCapacity += nAdditionalCnt; /*최대 용량 갱신*/
        for (i = nUsingCnt; i < nCapacity; i++) {
            pObject[i].pPrev = &pObject[i - 1]; /*연결리스트 재구축*/
            pObject[i].pNext = &pObject[i + 1];
            pObject[i].bActivate = FALSE;
        }
        pObject[nUsingCnt].pPrev = NULL; /*어차피 남는게 없어서 만들어진것이기 때문에 비워놓는다*/
        pObject[i - 1].pNext = NULL;
        pInactivate = &pObject[nUsingCnt]; /*재구축된 오브젝트 링크 포인팅*/
#else
        return NULL; /*버그가 수정 될 때 까지는 확장 금지.*/
#endif
    }
    /*비활성화 리스트에서 제거*/
    pTmp = pInactivate;
    pInactivate = pInactivate->pNext;   /*놀고있는 오브젝트 리스트에서 잘라낸다*/
    memset(pTmp, 0, sizeof(T));         /*발급 전 초기화*/
    pTmp->bActivate = TRUE;             /*활성화 플래그 ON*/

    /*활성화 리스트에 체결*/
    pTmp->pNext = pActivate;
    pActivate = pTmp;                   /*체결*/
    pActivate->pPrev = NULL;            /*이 노드는 제일 전방에 들어갈것이므로 이전노드가 없다.*/
    if (pActivate->pNext) pActivate->pNext->pPrev = pActivate; /*맨 앞 노드의 바로 다음 노드 prev 포인터 체결*/
    nUsingCnt++;
    return pTmp; /*깔끔하게 초기화된 오브젝트 반환*/
}

/**
    @brief  지정된 오브젝트를 풀에 반환
*/
template <typename T> void ObjectPool<T>::deactivateObject(T* pObj)
{
    T* pTmp;

    if (!nUsingCnt) return;       /*이미 전부 반환되어있는 경우 이 요청은 무시한다*/
    if (!pObj->bActivate) return; /*이미 비활성화 되어있는 경우 이 요청은 무시*/

    /*활성화 리스트에서 제거*/
    if (!pObj->pPrev) /*비활성화 하려는 노드가 머리 노드인경우*/
        pActivate = pObj->pNext; /*다음 노드를 머리 노드로 등록*/

    else if (!pObj->pNext) /*비활성화 하려는 노드가 끄트머리 노드인경우*/
        pObj->pPrev->pNext = NULL; /*이전 노드의 pNext 만 비워준다*/

    else { /*비활성화 하려는 노드가 중간에 끼인 노드인 경우*/
        pObj->pPrev->pNext = pObj->pNext;
        pObj->pNext->pPrev = pObj->pPrev; /*앞노드 뒷노드를 서로 체결*/
    }

    /*비활성화 리스트에 삽입*/
    pTmp = pInactivate;             /*비활성화 리스트 머리 노드 얻어오기 (NULL일수도 있음)*/
    pInactivate = pObj;             /*비활성화 리스트 머리 노드 갱신*/
    pInactivate->pNext = pTmp;      /*체결 완료*/
    pInactivate->bActivate = FALSE; /*활성화 플래그 OFF*/
    nUsingCnt--;
}

template <typename T> ObjectPoolList<T>::ObjectPoolList()
{
    pPoolList = 0;
    nCapacity = 0;
    nUsingCnt = 0;
    nTotalPoolsSize = 0;
    nTotalUsingSize = 0;
}

template <typename T> ObjectPoolList<T>::~ObjectPoolList() {}

/**
    @brief 오브젝트풀을 여러개 만들어서 발급해주는 일종의 매니저
*/
template <typename T> BOOL ObjectPoolList<T>::Init(int nPoolCnt, int nObjCnt)
{
    int i;

    nTotalPoolsSize = sizeof(ObjectPool<T>) * nPoolCnt;
    nCapacity = nPoolCnt;
    pPoolList = (ObjectPool<T>**)malloc(nTotalPoolsSize);
    if (!pPoolList) return FALSE;

    /*풀 리스트 생성후 미리 초기화 해두기*/
    for (i = 0; i < nPoolCnt; i++) {
        pPoolList[i] = new ObjectPool<T>;
        pPoolList[i]->Init(nObjCnt);
    }
    nTotalUsingSize = pPoolList[0]->getUsingMemoryBytes() * nPoolCnt;
    nUsingCnt = 0; /*현재 사용중인 풀 갯수 0으로 초기화*/
    return TRUE;
}

/**
    @brief 생성된 풀 리스트에서 풀을 하나 얻어온다.
*/
template <typename T> ObjectPool<T>* ObjectPoolList<T>::getPool()
{
    ObjectPool<T>* p;

    if (nUsingCnt >= nCapacity) return NULL; /*남은 풀이 없으면 NULL*/
    p = pPoolList[nUsingCnt];
    p->Reset();                              /*풀 자체 연결리스트 재구축*/
    p->setAssigned(TRUE);                    /*풀 사용 플래그 ON*/
    nUsingCnt++;
    return p;
}

/**
    @brief 풀 리스트에 풀을 반환한다.
*/
template <typename T> void ObjectPoolList<T>::returnPool(ObjectPool<T>* pPool)
{
    if (nUsingCnt <= 0) return;       /*이미 전부 반환 상태라면 이 요청은 무시된다*/
    if (!pPool->isAssigned()) return; /*이미 비활성화된 풀은 반환하지 않음*/
    nUsingCnt--;
    pPool->setAssigned(FALSE);        /*풀 사용 플래그 OFF*/
    pPoolList[nUsingCnt] = pPool;     /*풀 반환*/
}