///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Wahoo Fitness. All Rights Reserved.
//
// The information contained herein is property of Wahoo Fitness LLC.
// Terms and conditions of usage are described in detail in the
// WAHOO FITNESS API LICENSE AGREEMENT.
//
// Licensees are granted free, non-transferable use of the information.
// NO WARRANTY of ANY KIND is provided.
// This heading must NOT be removed from the file.
///////////////////////////////////////////////////////////////////////////////
//
//  BTGlucoseDetailVC.h
//  WahooDemo
//
//  Created by Michael Moore on 2/23/12.
//  Copyright (c) 2012 Wahoo Fitness. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WFConnector/WFConnector.h>


@interface BTGlucoseDetailVC : UIViewController
{
    WFBTLEGlucoseData* glucoseRecord;
    
    UILabel* sequenceLabel;
    UILabel* baseTimeLabel;
    UILabel* timeOffsetLabel;
    UILabel* concentrationLabel;
    UILabel* sampleTypeLabel;
    UILabel* sampleLocationLabel;
    UILabel* statusLabel;
}


@property (nonatomic, retain) WFBTLEGlucoseData* glucoseRecord;
@property (nonatomic, retain) IBOutlet UILabel* sequenceLabel;
@property (nonatomic, retain) IBOutlet UILabel* baseTimeLabel;
@property (nonatomic, retain) IBOutlet UILabel* timeOffsetLabel;
@property (nonatomic, retain) IBOutlet UILabel* concentrationLabel;
@property (nonatomic, retain) IBOutlet UILabel* sampleTypeLabel;
@property (nonatomic, retain) IBOutlet UILabel* sampleLocationLabel;
@property (nonatomic, retain) IBOutlet UILabel* statusLabel;

@end
