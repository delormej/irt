//
//  BTLEWeightScaleUserViewController.m
//  WahooDemo
//
//  Created by Benjamin Johnston on 6/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "BTLEWeightScaleUserViewController.h"

@interface BTLEWeightScaleUserViewController ()

@end

@implementation BTLEWeightScaleUserViewController
@synthesize initialsField;
@synthesize heightField;
@synthesize goalWeightField;
@synthesize upperRangeField;
@synthesize lowerRangeField;
@synthesize toleranceField;
@synthesize userIdLabel;
@synthesize submitButton;
@synthesize delegate;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
}

- (void)viewDidUnload
{
    [self setInitialsField:nil];
    [self setHeightField:nil];
    [self setGoalWeightField:nil];
    [self setUpperRangeField:nil];
    [self setLowerRangeField:nil];
    [self setToleranceField:nil];
    [self setUserIdLabel:nil];
    [self setDelegate:nil];
    [self setSubmitButton:nil];
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)dealloc {
    [initialsField release];
    [heightField release];
    [goalWeightField release];
    [upperRangeField release];
    [lowerRangeField release];
    [toleranceField release];
    [userIdLabel release];
    [delegate release];
    [submitButton release];
    [super dealloc];
}

#pragma mark - User Interface Actions

- (IBAction)submitButtonTapped:(id)sender {
    
    //send details back to last page (delegate)
    BOOL retVal = [self.delegate createUserRecordWithInitials:self.initialsField.text
                                                    minWeight:[self.lowerRangeField.text floatValue]
                                                    maxWeight:[self.upperRangeField.text floatValue]
                                                       height:[self.heightField.text intValue]
                                                   goalWeight:[self.goalWeightField.text floatValue]
                                            gainLossTolerance:[self.toleranceField.text floatValue]
                   ];
    
    
    if (retVal) 
    {
        //data seems valid, go ahead and dismiss the modal VC
        [self dismissModalViewControllerAnimated:YES];
    }
    else 
    {
        //invalid data detected by API.
        NSLog(@"Did not add user record");
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"WeightScale" message:@"Did not add user record. Check User ID > 0, Upper range > Lower range, Lower range > 0." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        [alert release];
        alert = nil;
    }
}


- (IBAction)cancelButtonTapped:(id)sender {
    [self dismissModalViewControllerAnimated:YES];
}

- (IBAction)textFieldDidEndOnExit:(id)sender {
    [sender resignFirstResponder];
}



@end
