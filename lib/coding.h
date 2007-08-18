#ifndef MARK_b7d77b2a_6c98_4b28_954e_957e416e0a19
#define MARK_b7d77b2a_6c98_4b28_954e_957e416e0a19

uint8_t *aff_encode_type(uint8_t *buf, uint32_t size, enum AffNodeType_e type);
uint8_t *aff_encode_u32(uint8_t *buf, uint32_t size, uint32_t data);
uint8_t *aff_encode_u64(uint8_t *buf, uint32_t size, uint64_t data);
uint8_t *aff_encode_double(uint8_t *buf, uint32_t size, double data);

#endif /* !defined(MARK_b7d77b2a_6c98_4b28_954e_957e416e0a19) */
