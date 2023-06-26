/****************************************************************************\
*
* This file is provided under the MIT license (see the following Web site
* for further details: https://mit-license.org/ ).
*
* Copyright © 2023 Thales Group
*
\****************************************************************************/

#ifndef __P11_TOOLKIT_H__
#define __P11_TOOLKIT_H__

#include <stdbool.h>

#include <cryptoki_v2.h>

#include "misc-toolkit.h"

/*
 * Definitions
 */
#define P11TK_OUID_LENGTH 12

// Slot.
typedef struct _SLOT
{
    CK_SLOT_ID slotId;
    CK_SLOT_INFO slotInfo;
    CK_TOKEN_INFO tokenInfo;
    CK_RV slotStatus;
    CK_RV tokenStatus;
} SLOT;

typedef SLOT *SLOT_PTR;
typedef SLOT_PTR SLOT_LIST;
typedef SLOT_LIST *SLOT_LIST_PTR;

// Slot list.
typedef CK_SLOT_ID_PTR SLOT_ID_LIST;
typedef SLOT_ID_LIST *SLOT_ID_LIST_PTR;

// getHaState() arguments.
typedef struct _GET_HA_STATE_ARGUMENTS
{
    CK_SLOT_ID slotId;
    CK_HA_STATUS haState;
} GET_HA_STATE_ARGUMENTS;

typedef GET_HA_STATE_ARGUMENTS *GET_HA_STATE_ARGUMENTS_PTR;

typedef struct _P11_ELLIPTIC_CURVES_LIST_ELEMENT
{
    size_t oidLength;
    int maximumDataLength;
    int fieldLength;
    unsigned char *oid;
    char *name;
} P11_ELLIPTIC_CURVES_LIST_ELEMENT;

/*
 * Interface
 */
CK_RV p11tk_closeSession(const CK_SESSION_HANDLE sessionHandle);

CK_RV p11tk_destroyObject(const CK_SESSION_HANDLE sessionHandle,
                          const CK_OBJECT_HANDLE objectHandle);

CK_RV p11tk_destroyObjectsForClass(const CK_SESSION_HANDLE sessionHandle,
                                   const CK_OBJECT_CLASS objectClass);

CK_RV p11tk_destroyObjectsForLabel(const CK_SESSION_HANDLE sessionHandle,
                                   const CK_OBJECT_CLASS objectClass,
                                   const CK_CHAR *const objectLabel);

CK_RV p11tk_destroyObjectsForObjectTemplate(const CK_SESSION_HANDLE sessionHandle,
                                            const CK_ATTRIBUTE *const objectTemplate,
                                            const size_t objectTemplateSize);

CK_RV p11tk_encryptWithAesKwp(const CK_SESSION_HANDLE sessionHandle,
                              const CK_OBJECT_HANDLE encryptionKeyHandle,
                              const CK_BYTE *const data,
                              const CK_ULONG dataLength,
                              CK_BYTE *const pEncryptedData,
                              CK_ULONG *const pEncryptedDataLength);

CK_RV p11tk_finalizeClientLibrary(void);

CK_RV p11tk_findObjectForLabel(const CK_SESSION_HANDLE sessionHandle,
                               const CK_OBJECT_CLASS objectClass,
                               const CK_CHAR *const objectLabel,
                               CK_OBJECT_HANDLE *const pObjectHandle);

CK_RV p11tk_findObjectForOuid(const CK_SESSION_HANDLE sessionHandle,
                              const CK_BYTE ouid[],
                              const size_t ouidLength,
                              CK_OBJECT_HANDLE *const pObjectHandle);

CK_RV p11tk_findObjectsFinal(const CK_SESSION_HANDLE sessionHandle);

CK_RV p11tk_generateEllipticKeyPair(const CK_SESSION_HANDLE sessionHandle,
                                    const CK_BBOOL isTokenObject,
                                    const unsigned int ellipticCurveIdentifier,
                                    const CK_CHAR *const publicKeyLabel,
                                    const CK_CHAR *const privateKeyLabel,
                                    CK_OBJECT_HANDLE *const pPublicKeyHandle,
                                    CK_OBJECT_HANDLE *const pPrivateKeyHandle);

CK_RV p11tk_generateStorageKey(const CK_SESSION_HANDLE sessionHandle,
                               const CK_BBOOL isTokenObject,
                               const CK_CHAR *const keyLabel,
                               CK_OBJECT_HANDLE *const pKeyHandle);

CK_RV p11tk_generateStorageKeyIfNeeded(const CK_SESSION_HANDLE sessionHandle,
                                       const CK_BBOOL isTokenObject,
                                       const CK_CHAR *const keyLabel,
                                       CK_OBJECT_HANDLE *const pKeyHandle);

CK_RV p11tk_getEllipticCurveDataForCurveIdentifier(const unsigned int ellipticCurveIdentifier,
                                                   const P11_ELLIPTIC_CURVES_LIST_ELEMENT **const pEllipticCurveData);

CK_RV p11tk_getEllipticCurveDataWithOid(const unsigned char *const oid,
                                        const size_t oidLength,
                                        const P11_ELLIPTIC_CURVES_LIST_ELEMENT **const pCurveData);

CK_RV p11tk_getHaState(const GET_HA_STATE_ARGUMENTS *const pHaStateArguments);

CK_RV p11tk_getObjectUniqueIdentifier(const CK_SESSION_HANDLE sessionHandle,
                                      const CK_SESSION_HANDLE objectHandle,
                                      CK_BYTE *const ouid,
                                      size_t *const pOuidLength);

CK_RV p11tk_getSlotInfo(const CK_SLOT_ID slotId,
                        const CK_SLOT_INFO *const pSlotInfo);

CK_RV p11tk_getSlotList(const CK_BBOOL isTokenPresent,
                        SLOT_LIST *const pSlotList,
                        CK_ULONG *const pSlotCount);

CK_RV p11tk_getTokenInfo(const CK_SLOT_ID slotId,
                         const CK_TOKEN_INFO *const pTokenInfo);

CK_RV p11tk_initializeClientLibrary(const CK_C_INITIALIZE_ARGS *const pInitializeArguments);

CK_RV p11tk_login(const CK_SESSION_HANDLE sessionHandle,
                  const CK_CHAR *const password,
                  const CK_ULONG passwordLength);

CK_RV p11tk_logout(const CK_SESSION_HANDLE sessionHandle);

CK_RV p11tk_openSession(const CK_SLOT_ID slotId,
                        CK_SESSION_HANDLE *const pSessionHandle);

CK_RV p11tk_prepare(const CK_C_INITIALIZE_ARGS *const pInitializeArguments,
                    const CK_SLOT_ID slotId,
                    const CK_CHAR *const password,
                    const CK_ULONG passwordLength,
                    CK_SESSION_HANDLE *const pSessionHandle);

void p11tk_printHaState(CK_HA_STATE_PTR const pHaState);

CK_RV p11tk_printHaStateWithSlotList(void);

CK_RV p11tk_printHaStateWithoutSlotList(void);

void p11tk_printSlotList(const SLOT_LIST slotList,
                         const CK_ULONG slotCount);

CK_RV p11tk_printSlotListStateWithoutSlotList(void);

void p11tk_printSlotListStateWithSlotList(const SLOT_LIST slotList,
                                          const CK_ULONG slotCount);

CK_RV p11tk_terminate(const CK_SESSION_HANDLE sessionHandle);

CK_RV p11tk_unwrapSensitiveData(const CK_SESSION_HANDLE sessionHandle,
                                const CK_OBJECT_HANDLE encryptionKeyHandle,
                                const CK_BYTE *const encryptedKey,
                                const CK_ULONG encryptedKeyLength,
                                const CK_BBOOL unwrappedKeyIsTokenObject,
                                const CK_ULONG unwrappedKeyLength,
                                const CK_CHAR *const unwrappedKeyLabel,
                                CK_OBJECT_HANDLE *const pUnwrappedKeyHandle);

void p11tk_writeError(const char *const message,
                      const CK_RV rv);

#endif /* __P11_TOOLKIT_H__ */