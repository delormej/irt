//
//  BTLEWeightScaleUserViewController.h
//  WahooDemo
//
//  Created by Benjamin Johnston on 6/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WFConnector/WFConnector.h>


@protocol BTLEWeightScaleUserDelegate <NSObject>

-(BOOL)createUserRecordWithInitials:(NSString*)initials
                          minWeight:(float)minWeightKg
                          maxWeight:(float)maxWeightKg
                             height:(float)heightCm
                         goalWeight:(float)goalWeightKg
                  gainLossTolerance:(float)toleranceKg;

@end

@interface BTLEWeightScaleUserViewController : UIViewController

@property (retain, nonatomic) id<BTLEWeightScaleUserDelegate> delegate;
@property (retain, nonatomic) IBOutlet UITextField *initialsField;
@property (retain, nonatomic) IBOutlet UITextField *heightField;
@property (retain, nonatomic) IBOutlet UITextField *goalWeightField;
@property (retain, nonatomic) IBOutlet UITextField *upperRangeField;
@property (retain, nonatomic) IBOutlet UITextField *lowerRangeField;
@property (retain, nonatomic) IBOutlet UITextField *toleranceField;
@property (retain, nonatomic) IBOutlet UILabel *userIdLabel;
@property (retain, nonatomic) IBOutlet UIButton *submitButton;

- (IBAction)submitButtonTapped:(id)sender;
- (IBAction)cancelButtonTapped:(id)sender;
- (IBAction)textFieldDidEndOnExit:(id)sender;

@end
