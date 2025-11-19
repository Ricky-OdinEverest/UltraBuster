// Fill out your copyright notice in the Description page of Project Settings.


#include "BusterTypes.h"



bool FBusterPlayerAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
