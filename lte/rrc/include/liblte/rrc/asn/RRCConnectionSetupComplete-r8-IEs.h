/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "./asn1c/ASN1_files/EUTRA-RRC-Definitions.asn"
 * 	`asn1c -gen-PER -fcompound-names -fnative-types`
 */

#ifndef	_RRCConnectionSetupComplete_r8_IEs_H_
#define	_RRCConnectionSetupComplete_r8_IEs_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include "DedicatedInfoNAS.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct RegisteredMME;
struct RRCConnectionSetupComplete_v8a0_IEs;

/* RRCConnectionSetupComplete-r8-IEs */
typedef struct RRCConnectionSetupComplete_r8_IEs {
	long	 selectedPLMN_Identity;
	struct RegisteredMME	*registeredMME	/* OPTIONAL */;
	DedicatedInfoNAS_t	 dedicatedInfoNAS;
	struct RRCConnectionSetupComplete_v8a0_IEs	*nonCriticalExtension	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RRCConnectionSetupComplete_r8_IEs_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RRCConnectionSetupComplete_r8_IEs;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "RegisteredMME.h"
#include "RRCConnectionSetupComplete-v8a0-IEs.h"

#endif	/* _RRCConnectionSetupComplete_r8_IEs_H_ */
#include <asn_internal.h>