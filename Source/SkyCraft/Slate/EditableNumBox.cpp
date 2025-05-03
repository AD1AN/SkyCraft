// ADIAN Copyrighted


#include "EditableNumBox.h"

void UEditableNumBox::HandleOnTextChanged(const FText& InText)
{
	Super::HandleOnTextChanged(InText);
	
	FString InputString = InText.ToString();
	FString CleanString;
	bool bDecimalPointFound = false;
	bool bNegativeSignFound = false;

	for (int32 Index = 0; Index < InputString.Len(); ++Index)
	{
		TCHAR Char = InputString[Index];

		if (FChar::IsDigit(Char))
		{
			CleanString.AppendChar(Char);
		}
		else if (bIsFloat && Char == '.' && !bDecimalPointFound)
		{
			// Only allow one decimal point
			CleanString.AppendChar(Char);
			bDecimalPointFound = true;
		}
		else if (Char == '-' && Index == 0 && !bNegativeSignFound)
		{
			// Allow negative sign at the start
			CleanString.AppendChar(Char);
			bNegativeSignFound = true;
		}
	}

	if (InputString != CleanString)
	{
		SetText(FText::FromString(CleanString));
	}
}

void UEditableNumBox::HandleOnTextCommitted(const FText& InText, ETextCommit::Type CommitMethod)
{
	Super::HandleOnTextCommitted(InText, CommitMethod);

	if (!bClampValue)
	{
		return;
	}

	FString TextString = InText.ToString();

	if (TextString.IsEmpty() || TextString == "-" || TextString == "." || TextString == "-.")
	{
		// Skip if the text is incomplete or invalid
		return;
	}

	float Number = FCString::Atof(*TextString);

	if (!bIsFloat)
	{
		// Round to nearest int if in integer mode
		Number = FMath::RoundToFloat(Number);
	}

	float ClampedValue = FMath::Clamp(Number, MinValue, MaxValue);

	if (!FMath::IsNearlyEqual(Number, ClampedValue))
	{
		SetText(FText::FromString(bIsFloat
			? FString::SanitizeFloat(ClampedValue)
			: FString::FromInt(static_cast<int32>(ClampedValue))));
	}
}
