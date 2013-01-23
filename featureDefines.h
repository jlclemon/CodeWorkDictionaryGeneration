/*Copyright (c) 2011, Texas Instruments Inc.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:
• Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
• Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.*/
#ifndef FEATURE_DEFINES_H
#define FEATURE_DEFINES_H

enum FeatureLocalizationAlgos
{
	FEATURE_LOCALIZATION_SIFT = 1,
	FEATURE_LOCALIZATION_SURF = 2,
	FEATURE_LOCALIZATION_FAST = 4,
	FEATURE_LOCALIZATION_HOG = 8,
	FEATURE_LOCALIZATION_GRID = 16,

	NUMBER_OF_FEATURE_LOCALIZATION_ALGOS=5

};

enum FeatureDescriptorAlgos
{
	FEATURE_DESC_SIFT = 1,
	FEATURE_DESC_SURF = 2,
	FEATURE_DESC_HOG = 8,
	FEATURE_DESC_BRIEF = 16,
	NUMBER_OF_FEATURE_DESC_ALGOS = 4

};




#endif