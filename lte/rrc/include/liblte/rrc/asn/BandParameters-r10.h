/*
 * Generated by asn1c-0.9.22 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "./asn1c/ASN1_files/EUTRA-RRC-Definitions.asn"
 * 	`asn1c -gen-PER -fcompound-names -fnative-types`
 */

#ifndef	_BandParameters_r10_H_
#define	_BandParameters_r10_H_


#include <liblte/rrc/asn/asn_application.h>

/* Including external dependencies */
#include <liblte/rrc/asn/NativeInteger.h>
#include <liblte/rrc/asn/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct BandParametersUL_r10;
struct BandParametersDL_r10;

/* BandParameters-r10 */
typedef struct BandParameters_r10 {
	long	 bandEUTRA_r10;
	struct BandParametersUL_r10	*bandParametersUL_r10	/* OPTIONAL */;
	struct BandParametersDL_r10	*bandParametersDL_r10	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BandParameters_r10_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BandParameters_r10;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "liblte/rrc/asn/BandParametersUL-r10.h"
#include "liblte/rrc/asn/BandParametersDL-r10.h"

#endif	/* _BandParameters_r10_H_ */
#include <liblte/rrc/asn/asn_internal.h>
