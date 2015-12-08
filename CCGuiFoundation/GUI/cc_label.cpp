#include "stdafx.h"
#include "cc_comctl.h"
#include "cc_label.h"
#include "cc_presentation.h"

using namespace cc::presentation::windows;
using namespace cc::presentation::element;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			namespace style
			{
				PassRefPtr<ILabelStyleController> Win8Style::CreateLabelStyle()
				{
					return adoptRef(new Win8LabelStyle);
				}
			}

			Label::Label(PassRefPtr<ILabelStyleController> _styleController)
				: Control(_styleController.get())
				, styleController(_styleController.get())
			{

			}

			Label::~Label()
			{

			}

			CColor Label::GetTextColor()
			{
				return textColor;
			}

			void Label::SetTextColor(CColor value)
			{
				if (textColor != value)
				{
					textColor = value;
					styleController->SetTextColor(textColor);
				}
			}

			namespace style
			{
				Win8LabelStyle::Win8LabelStyle()
				{
					textElement = SolidLabelElement::Create();
					textElement->SetColor(GetDefaultTextColor());

					boundsComposition = adoptRef(new BoundsComposition);
					boundsComposition->SetMinSizeLimitation(BoundsComposition::LimitToElementAndChildren);
					boundsComposition->SetOwnedElement(textElement);
				}

				Win8LabelStyle::~Win8LabelStyle()
				{

				}

				PassRefPtr<BoundsComposition> Win8LabelStyle::GetBoundsComposition()
				{
					return boundsComposition;
				}

				PassRefPtr<Composition> Win8LabelStyle::GetContainerComposition()
				{
					return boundsComposition;
				}

				void Win8LabelStyle::SetFocusableComposition(PassRefPtr<Composition> value)
				{

				}

				void Win8LabelStyle::SetText(const CString& value)
				{
					textElement->SetText(value);
				}

				void Win8LabelStyle::SetFont(const Font& value)
				{
					textElement->SetFont(value);
				}

				void Win8LabelStyle::SetVisuallyEnabled(bool value)
				{

				}

				CColor Win8LabelStyle::GetDefaultTextColor()
				{
					return Win8WindowStyle::GetSystemColor(Win8WindowStyle::CT_Text_Enable);
				}

				void Win8LabelStyle::SetTextColor(CColor value)
				{
					textElement->SetColor(value);
				}
			}
		}
	}
}