//
//  wf_btle_glucose_types.h
//  WFConnector
//
//  Created by Michael Moore on 2/22/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#ifndef _wf_btle_glucose_types_h_
#define _wf_btle_glucose_types_h_


typedef struct
{
    BOOL bLowBatteryDetect;
    BOOL bSensorMalfunctionDetect;
    BOOL bSampleSize;
    BOOL bStripInsertErrorDetect;
    BOOL bStripTypeErrorDetect;
    BOOL bResultHighLowDetect;
    BOOL bTempHighLowDetect;
    BOOL bReadInterruptDetect;
    BOOL bGeneralDeviceFault;
    BOOL bTimeFault;
    BOOL bMultipleBond;
    
} WFBTLEGlucoseDeviceFeatures_t;

typedef enum
{
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_UNDEFINED = 0,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_CAPILLARY_WHOLE_BLOOD,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_CAPILLARY_PLASMA,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_VENOUS_WHOLE_BLOOD,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_VENOUS_PLASMA,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_ARTERIAL_WHOLE_BLOOD,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_ARTERIAL_PLASMA,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_UNDETERMINED_WHOLE_BLOOD,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_UNDETERMINED_PLASMA,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_INTERSTITIAL_FLUID,
    WF_BTLE_GLUCOSE_SAMPLE_TYPE_CONTROL_SOLUTION,
    
} WFBTLEGlucoseSampleType_t;

typedef enum
{
    WF_BTLE_GLUCOSE_SAMPLE_LOC_UNDEFINED = 0,
    WF_BTLE_GLUCOSE_SAMPLE_LOC_FINGER,
    WF_BTLE_GLUCOSE_SAMPLE_LOC_ALTERNATE_TEST_SITE,
    WF_BTLE_GLUCOSE_SAMPLE_LOC_EARLOBE,
    WF_BTLE_GLUCOSE_SAMPLE_LOC_CONTROL_SOLUTION,
    WF_BTLE_GLUCOSE_SAMPLE_LOC_NOT_AVAILABLE = 15,
    
} WFBTLEGlucoseSampleLocation_t;

typedef union
{
    USHORT usStatus;
    struct
    {
        BOOL bBatteryLow              : 1;
        BOOL bSensorMalfunction       : 1;
        BOOL bSampleSizeInsufficient  : 1;
        BOOL bStripInsertionError     : 1;
        BOOL bStripTypeIncorrect      : 1;
        BOOL bResultToHigh            : 1;
        BOOL bResultToLow             : 1;
        BOOL bTemperatureToHigh       : 1;
        BOOL bTemperatureToLow        : 1;
        BOOL bReadInterrupted         : 1;
        BOOL bGeneralDeviceFault      : 1;
        BOOL bTimeFault               : 1;
        BOOL                          : 1;
        BOOL                          : 1;
        BOOL                          : 1;
        BOOL                          : 1;
    };
    
} WFBTLEGlucoseSensorStatus_t;

typedef struct
{
    USHORT usSequence;
    NSTimeInterval baseTime;
    SSHORT ssTimeOffset;
    float concentration;
    WFBTLEGlucoseSampleType_t sampleType;
    WFBTLEGlucoseSampleLocation_t sampleLocation;
    WFBTLEGlucoseSensorStatus_t sensorStatus;
    
} WFBTLEGlucoseMeasurementData_t;

typedef enum
{
    WF_BTLE_GLUCOSE_CARB_ID_UNDEFINED = 0,
    WF_BTLE_GLUCOSE_CARB_ID_BREAKFAST,
    WF_BTLE_GLUCOSE_CARB_ID_LUNCH,
    WF_BTLE_GLUCOSE_CARB_ID_DINNER,
    WF_BTLE_GLUCOSE_CARB_ID_SNACK,
    WF_BTLE_GLUCOSE_CARB_ID_DRINK,
    WF_BTLE_GLUCOSE_CARB_ID_SUPPER,
    WF_BTLE_GLUCOSE_CARB_ID_BRUNCH,
    
} WFBTLEGlucoseCarbId_t;

typedef enum
{
    WF_BTLE_GLUCOSE_MEAL_INFO_UNDEFINED = 0,
    WF_BTLE_GLUCOSE_MEAL_INFO_BEFORE_MEAL,
    WF_BTLE_GLUCOSE_MEAL_INFO_AFTER_MEAL,
    WF_BTLE_GLUCOSE_MEAL_INFO_FASTING,
    WF_BTLE_GLUCOSE_MEAL_INFO_CASUAL,
    WF_BTLE_GLUCOSE_MEAL_INFO_BEDTIME,
    
} WFBTLEGlucoseMealInfo_t;

typedef enum
{
    WF_BTLE_GLUCOSE_TESTER_INFO_UNDEFINED = 0,
    WF_BTLE_GLUCOSE_TESTER_INFO_SELF,
    WF_BTLE_GLUCOSE_TESTER_INFO_HEALTH_CARE_PRO,
    WF_BTLE_GLUCOSE_TESTER_INFO_LAB_TEST,
    WF_BTLE_GLUCOSE_TESTER_INFO_NOT_AVAILABLE = 15,
    
} WFBTLEGlucoseTesterInfo_t;

typedef enum
{
    WF_BTLE_GLUCOSE_HEALTH_INFO_UNDEFINED = 0,
    WF_BTLE_GLUCOSE_HEALTH_INFO_MINOR_HEALTH_ISSUES,
    WF_BTLE_GLUCOSE_HEALTH_INFO_MAJOR_HEALTH_ISSUES,
    WF_BTLE_GLUCOSE_HEALTH_INFO_DURING_MENSES,
    WF_BTLE_GLUCOSE_HEALTH_INFO_UNDER_STRESS,
    WF_BTLE_GLUCOSE_HEALTH_INFO_NO_HEALTH_ISSUES,
    WF_BTLE_GLUCOSE_HEALTH_INFO_NOT_AVAILABLE = 15,
    
} WFBTLEGlucoseHealthInfo_t;

typedef enum
{
    WF_BTLE_GLUCOSE_MED_ID_UNDEFINED = 0,
    WF_BTLE_GLUCOSE_MED_ID_RAPID_ACTING_INSULIN,
    WF_BTLE_GLUCOSE_MED_ID_SHORT_ACTING_INSULIN,
    WF_BTLE_GLUCOSE_MED_ID_IMMEDIATE_ACTING_INSULIN,
    WF_BTLE_GLUCOSE_MED_ID_LONG_ACTING_INSULIN,
    WF_BTLE_GLUCOSE_MED_ID_PRE_MIXED_INSULIN,
    
} WFBTLEGlucoseMedId_t;

typedef enum
{
    WF_BTLE_GLUCOSE_MED_UNITS_MILLIGRAMS = 0,
    WF_BTLE_GLUCOSE_MED_UNITS_MILLILITERS,
    
} WFBTLEGlucoseMedUnits_t;

typedef struct
{
    USHORT usSequence;
    WFBTLEGlucoseCarbId_t carbohydrateId;
    float carbohydrate;
    WFBTLEGlucoseMealInfo_t mealInfo;
    WFBTLEGlucoseTesterInfo_t testerInfo;
    WFBTLEGlucoseHealthInfo_t healthInfo;
    USHORT usExerciseDuration;
    UCHAR ucExerciseIntensity;
    WFBTLEGlucoseMedId_t medicationId;
    float medication;
    WFBTLEGlucoseMedUnits_t medicationUnits;
    float hbA1c;
    
} WFBTLEGlucoseMeasurementContext_t;



typedef enum
{
    WF_BTLE_GLUCOSE_REC_OP_CODE_UNDEFINED = 0,
    WF_BTLE_GLUCOSE_REC_OP_CODE_REPORT_STORED_RECORDS,
    WF_BTLE_GLUCOSE_REC_OP_CODE_DELETE_STORED_RECORDS,
    WF_BTLE_GLUCOSE_REC_OP_CODE_ABORT_OPERATION,
    WF_BTLE_GLUCOSE_REC_OP_CODE_REPORT_RECORD_COUNT,
    WF_BTLE_GLUCOSE_REC_OP_CODE_RECORD_COUNT_RESPONSE,
    WF_BTLE_GLUCOSE_REC_OP_CODE_RESPONSE_CODE,
    
} WFBTLEGlucoseRecordOpCode_t;

typedef enum
{
    WF_BTLE_GLUCOSE_REC_OPERATOR_NULL = 0,
    WF_BTLE_GLUCOSE_REC_OPERATOR_ALL_RECORDS,
    WF_BTLE_GLUCOSE_REC_OPERATOR_LESS_THAN_OR_EQUAL,
    WF_BTLE_GLUCOSE_REC_OPERATOR_GREATER_THAN_OR_EQUAL,
    WF_BTLE_GLUCOSE_REC_OPERATOR_WITHIN_RANGE,
    WF_BTLE_GLUCOSE_REC_OPERATOR_FIRST_RECORD,
    WF_BTLE_GLUCOSE_REC_OPERATOR_LAST_RECORD,
    
} WFBTLEGlucoseRecordOperator_t;


#endif // _wf_btle_glucose_types_h_
