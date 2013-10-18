//
//  wf_bike_trainer_types.h
//  WFConnector
//
//  Created by Michael Moore on 8/18/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#ifndef WFConnector_wf_bike_trainer_types_h
#define WFConnector_wf_bike_trainer_types_h


/** Describes the difficulty level of trainer progressive resistance while in Standard Mode. */
typedef enum
{
    /** Trainer level not specified. */
    WF_BIKE_TRAINER_LEVEL_NONE = 0,
    /** The least difficult progressive power curve. */
    WF_BIKE_TRAINER_LEVEL_1    = 1,
    /** Progressive resistance level 2 of 9. */
    WF_BIKE_TRAINER_LEVEL_2    = 2,
    /** Progressive resistance level 3 of 9. */
    WF_BIKE_TRAINER_LEVEL_3    = 3,
    /** Progressive resistance level 4 of 9. */
    WF_BIKE_TRAINER_LEVEL_4    = 4,
    /** Progressive resistance level 5 of 9. */
    WF_BIKE_TRAINER_LEVEL_5    = 5,
    /** Progressive resistance level 6 of 9. */
    WF_BIKE_TRAINER_LEVEL_6    = 6,
    /** Progressive resistance level 7 of 9. */
    WF_BIKE_TRAINER_LEVEL_7    = 7,
    /** Progressive resistance level 8 of 9. */
    WF_BIKE_TRAINER_LEVEL_8    = 8,
    /** The most difficult progressive power curve. */
    WF_BIKE_TRAINER_LEVEL_9    = 9,

    
} WFBikeTrainerLevel_t;

/** Describes the mode in which the trainer is currently running. */
typedef enum
{
    /** Trainer mode not specified. */
    WF_BIKE_TRAINER_MODE_NONE = 0,
    /** Standard Mode will mimic a typical fluid trainer's resistance curve. */
    WF_BIKE_TRAINER_MODE_STANDARD,
    /** Erg mode will maintain a constant target power output regardless of speed. */
    WF_BIKE_TRAINER_MODE_ERG,
    /** Sim mode will simulate real world riding conditions. */
    WF_BIKE_TRAINER_MODE_SIM,
    /** Resistance mode will allow direct control of the trainer's resistance. */
    WF_BIKE_TRAINER_MODE_RESISTANCE,
    
} WFBikeTrainerMode_t;


#endif  // WFConnector_wf_bike_trainer_types_h
