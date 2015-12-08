#ifndef CC_LABEL
#define CC_LABEL

#include "stdafx.h"
#include "cc_base.h"
#include "cc_interface.h"
#include "cc_control.h"
#include "cc_element.h"

using namespace cc::base;
using namespace cc::interfaces::windows;
using namespace cc::presentation::control;
using namespace cc::presentation::element;

namespace cc
{
	namespace presentation
	{
		namespace control
		{
			namespace style
			{
				class ILabelStyleController : public IControlStyleController
				{
				public:
					virtual CColor						GetDefaultTextColor() = 0;
					virtual void						SetTextColor(CColor value) = 0;
				};
			}

			class Label : public Control
			{
			public:
				Label(PassRefPtr<ILabelStyleController> _styleController);
				~Label();

				CColor									GetTextColor();
				void									SetTextColor(CColor value);

			protected:
				CColor									textColor;
				RefPtr<ILabelStyleController>			styleController;
			};

			namespace style
			{
				class Win8LabelStyle : public ILabelStyleController
				{
				public:
					Win8LabelStyle();
					~Win8LabelStyle();

					PassRefPtr<BoundsComposition>				GetBoundsComposition()override;
					PassRefPtr<Composition>						GetContainerComposition()override;
					void										SetFocusableComposition(PassRefPtr<Composition> value)override;
					void										SetText(const CString& value)override;
					void										SetFont(const Font& value)override;
					void										SetVisuallyEnabled(bool value)override;
					CColor										GetDefaultTextColor()override;
					void										SetTextColor(CColor value)override;

				protected:
					RefPtr<BoundsComposition>					boundsComposition;
					RefPtr<SolidLabelElement>					textElement;
				};
			}
		}
	}
}

#endif