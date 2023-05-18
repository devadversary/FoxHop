#pragma once

#include "windows.h"
#define MAX_TREE_DEPTH (256)             /*�ִ� Ʈ�� ����*/
#define TREENODE_ALLOCED_NOUSERDATA ((void*)-1)  /*Ʈ���� ��忡 ���������Ͱ� ������� ����*/

/**
    @brief Ʈ�� ��� ����ü
*/
typedef struct _node_tree
{
    BYTE               value;   /**< Ž���� ǥ��*/
    struct _node_tree* pSub;    /**< ���� Ʈ�����*/
    struct _node_tree* pNext;   /**< ���� ������ ���� Ʈ�����*/
    void*              pParam;  /**< �߰� ��� ����*/
}NODE_TREE;

/**
    @brief Ʈ�� ��ü
*/
typedef struct _tree
{
    NODE_TREE* pRoot;     /**< ��Ʈ���*/
    int        nElements; /**< �� ������ ����*/
    /*TODO : �޸�Ǯ �ȿ��� Ʈ���� �����ǵ��� �����ؾ� ��. (������ ����)*/
}TREE;

/**
    @brief Ʈ�� ��ȸ ��Ȳ ������ (����ڰ� ���� �ٷ� ���� ����)
*/
typedef enum _e_TreeEnum
{
    eTreeEnum_Start = 0, /**< ��ȸ �۾� �� �� ����*/
    eTreeEnum_Sub,       /**< ���� ��� ��ȸ �ؾ���*/
    eTreeEnum_Next,      /**< ���� ��� ��ȸ �ؾ���*/
    eTreeEnum_End        /**< ��ȸ �Ϸ� ����*/
}eTreeEnum;

/**
    @brief Ʈ�� ��ȸ�� ����� ��ȸ ���ǥ
*/
typedef struct _TreeEnum
{
    NODE_TREE* pNode;     /**< ��ȸ���� ���*/
    eTreeEnum  EnumState; /**< ��ȸ ����*/
}TREE_ENUM;

/**
    @brief Ʈ�� ��ȸ��, ������ ã�������� ������ �Լ� �ݹ�
*/
typedef void(*pfnTreeEnum)(NODE_TREE* pNode, void* pParam);

/**
    @brief Ʈ�� ��� �ʱ�ȭ
    @return �Ҵ� ���н� NULL
*/
BOOL TREE_Init(TREE* pTree);

/**
    @brief Ʈ���� �� �����͸� �ִ´�.
    @return ���Ե� ����� ������
    @remark pData�� NULL �̶��, ����� pParam�� TREENODE_ALLOCED_NOUSERDATA �� �ʱ�ȭ �ȴ�.
    @n      �ߺ� �������ǰ�� ���� ���� ���� �ʰ� NULL �� ��ȯ�Ѵ�.
*/
NODE_TREE* TREE_Input(TREE* pTree, void* pKey, int nKeyLen, void* pData);

/**
    @brief Ʈ������ �����͸� �����Ѵ�.
    @return �����Ǹ� TRUE, ������ �����Ͱ� ������ FALSE
    @remark Ʈ���� ��ϵ� �����͵�(pParam)  ���� �����ȴ�.
*/
BOOL TREE_Delete(TREE* pTree, void* pKey, int nKeyLen);

/**
    @brief Ʈ�� �˻� �Լ�
*/
NODE_TREE* TREE_Search(TREE* pTree, void* pKey, int nKeyLen);

/**
    @brief Ʈ�� ��ȸ
    @param pTree          ��ȸ�� Ʈ���� �ڵ�
    @param pfnCallback    ��ȸ�� ������ �߽߰� ȣ���� �ݹ� (�ʿ� ������ NULL)
    @param pCallbackParam �ݹ鿡 ������ ����
    @return �߰��� ������ ����
*/
int TREE_Enumerate(TREE* pTree, pfnTreeEnum pfnCallback, void* pCallbackParam);
