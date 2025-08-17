// ADIAN Copyrighted


#include "AdianText.h"

void UAdianText::SetTextAndOutlineColor(FLinearColor InColor, bool bOutlineFaded)
{
	SetColorAndOpacity(InColor);
	if (bOutlineFaded)
	{
		InColor.A = 0.5f;
		SetOutlineColor(InColor);
	}
	else SetOutlineColor(InColor);
}

void UAdianText::SetOutlineColor(FLinearColor InColor)
{
	FSlateFontInfo TempFont = GetFont();
	TempFont.OutlineSettings.OutlineColor = InColor;
	SetFont(TempFont);
}
