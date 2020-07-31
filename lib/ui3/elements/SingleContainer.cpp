#include '../UI.cpp';
#include '../../math/math.cpp';
#include '../utils/ElementStack.cpp';
#include 'Element.cpp';
#include 'LockedContainer.cpp';

class SingleContainer : LockedContainer
{
	
	/**
	 * @class LockedContainer
	 * @brief A Container that can only hold a single child
	 */
	
	protected Element@ _content;
	
	SingleContainer(UI@ ui, Element@ content, const string &in type_identifier)
	{
		super(ui, type_identifier);
		
		@this.content = content;
	}
	
	Element@ content
	{
		get { return @_content; }
		set
		{
			if(@_content == @value)
				return;
			
			if(@_content != null)
			{
				Container::remove_child(_content);
			}
			
			@_content = @value;
			
			if(@_content != null)
			{
				Container::add_child(_content);
			}
			
			_validate_layout = true;
		}
	}
	
	void fit_to_contents(float padding_x=NAN, float padding_y=NAN)
	{
		fit_to_contents(true, padding_x, padding_y);
	}
	
	void fit_to_contents(bool include_border, float padding_x=NAN, float padding_y=NAN)
	{
		if(is_nan(padding_x))
			padding_x = ui.style.spacing;
		if(is_nan(padding_y))
			padding_y = ui.style.spacing;
		
		if(@_content == null)
		{
			width  = padding_x * 2;
			height = padding_y * 2;
			return;
		}
		
		float width  = _content._width  + padding_x * 2;
		float height = _content._height + padding_y * 2;
		
		if(include_border)
		{
			width  += border_size * 2;
			height += border_size * 2;
		}
		
		this.width  = width;
		this.height = height;
	}
	
	void _queue_children_for_layout(ElementStack@ stack) override
	{
		if(@_content != null)
		{
			stack.push(_content);
		}
	}
	
	bool add_child(Element@ child) override
	{
		return false;
	}
	
	bool remove_child(Element@ child) override
	{
		if(Container::remove_child(child))
		{
			@_content = null;
			return true;
		}
		
		return false;
	}
	
	protected float border_size { get const { return 0; } }
	
}