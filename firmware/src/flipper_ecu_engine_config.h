#pragma once

typedef enum { CKPSPolatityRasing, CKPSPolatityFalling } CKPSPolatity;

typedef struct {
    CKPSPolatity ckps_polarity;
} FlipperECUEngineConfig;
