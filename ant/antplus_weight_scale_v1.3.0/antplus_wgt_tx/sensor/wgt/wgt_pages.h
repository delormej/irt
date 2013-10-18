/*
This software is subject to the license described in the License.txt file
included with this software distribution. You may not use this file except in compliance
with this license.

Copyright (c) Dynastream Innovations Inc. 2013
All rights reserved.
*/


#ifndef WGT_PAGES_H_
#define WGT_PAGES_H_

typedef union
{
   UCHAR ucCapabilities;
   struct
   {
      BOOL bScaleSet          : 1;  // User profile has been selected on the scale
      BOOL bScaleExchange     : 1;  // Scale can receive user profile (1 = can receive, 0 = can not)
      BOOL bAntfs             : 1;  // Scale ANT-FS channel is available
      UCHAR ucReserved        : 4;  // Reserved
      BOOL bNoDisplayExchange : 1;  // Display is NOT capable of user profile exchange (0 = can receive, 1 = can not)
   } stCapabilitiesBitField;
} WEIGHT_CAPABILITIES;

typedef union
{
   UCHAR ucAgeGender;
   struct 
   {
      UCHAR ucAge             : 7;  // Age ( 0 - 127 years)
      BOOL bGender            : 1;  // Gender
   } stAgeGender;      
}  WEIGHT_AGE_GENDER;

typedef union
{
   UCHAR ucDescriptiveBitField;
   struct
   {
      UCHAR ucActivityLevel   : 3;  // Activity Level
      UCHAR ucReserved        : 4;  // Reserved
      BOOL bAthlete           : 1;  // Athlete setting (1 = lifetime athlete, 0 = standard)
   } stDescriptiveBitField;
} WEIGHT_DESCRIPTION;

typedef struct
{
   USHORT usID;               // Unique user identifier (this field is used in the other weight data pages)
   WEIGHT_CAPABILITIES stCapabilities; // Capabilities bit field (this field might be used in other pages)
   WEIGHT_AGE_GENDER stUserInfo;   // Age and gender
   UCHAR ucHeight;            // Height (cm)
   WEIGHT_DESCRIPTION stDescr;   // Descriptive bit field
} WGTPage_UserProfile;         // Page 58 (User Profile)

typedef struct
{
   USHORT usBodyWeight100;    //  Body weight (1/100 kg)   
} WGTPage_BodyWeight;         // Page 1

typedef struct
{
   USHORT usHydrationP100;    // Hydration (0.01%)
   USHORT usBodyFatP100;      // Body fat (0.01%);
} WGTPage_BodyComposition;    // Page 2

typedef struct
{
   USHORT usActiveMetRate4;   // Active metabolic rate (1/4 kcal)
   USHORT usBasalMetRate4;    // Basal metabolic rate (1/4 kcal) 
} WGTPage_MetabolicInfo;      // Page 3

typedef struct
{
   USHORT usMuscleMass100;    // Muscle mass (1/100 kg)
   UCHAR ucBoneMass10;        // Bone mass (1/10 kg)   
} WGTPage_BodyMass;           // Page 4

#endif /*WGT_PAGES_H_*/
