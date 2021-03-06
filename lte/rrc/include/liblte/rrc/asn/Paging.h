/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "./asn1c/ASN1_files/EUTRA-RRC-Definitions.asn"
 * 	`asn1c -gen-PER -fcompound-names -fnative-types`
 */

#ifndef	_Paging_H_
#define	_Paging_H_


#include <liblte/rrc/asn/asn_application.h>

/* Including external dependencies */
#include <liblte/rrc/asn/NativeEnumerated.h>
#include <liblte/rrc/asn/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum Paging__systemInfoModification {
	Paging__systemInfoModification_true	= 0
} e_Paging__systemInfoModification;
typedef enum Paging__etws_Indication {
	Paging__etws_Indication_true	= 0
} e_Paging__etws_Indication;

/* Forward declarations */
struct PagingRecordList;
struct Paging_v890_IEs;

/* Paging */
typedef struct Paging {
	struct PagingRecordList	*pagingRecordList	/* OPTIONAL */;
	long	*systemInfoModification	/* OPTIONAL */;
	long	*etws_Indication	/* OPTIONAL */;
	struct Paging_v890_IEs	*nonCriticalExtension	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Paging_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_systemInfoModification_3;	// (Use -fall-defs-global to expose) */
/* extern asn_TYPE_descriptor_t asn_DEF_etws_Indication_5;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_Paging;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "liblte/rrc/asn/PagingRecordList.h"
#include "liblte/rrc/asn/Paging-v890-IEs.h"

#endif	/* _Paging_H_ */
#include <liblte/rrc/asn/asn_internal.h>
