/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ConfigurableScrollBar.h"
#include "RangePolicy.h"

#include <qscrollbar.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qcombobox.h>

#include <iostream>
using namespace std;

ConfigurableScrollBar::ConfigurableScrollBar (QWidget *parent, 
					      const char *name)
  : QVBox (parent, name)
{
  QHBox* hbox = new QHBox (this);

  /* **************************************************************
   *
   *  parameter name text entry
   *
   * ************************************************************** */
     
  new QLabel ("Name:", hbox);
  parameter_name = new QLineEdit (hbox);

  default_palette = parameter_name->palette();
  highlight_palette = default_palette;
  highlight_palette.setColor( QColorGroup::Base, Qt::yellow );

  connect (parameter_name, SIGNAL(lostFocus()),
	   this, SLOT(parameter_name_CB()));
  connect (parameter_name, SIGNAL(returnPressed()),
	   this, SLOT(parameter_name_CB()));
  connect (parameter_name, SIGNAL( textChanged(const QString&) ),
	   this, SLOT( text_changed_CB(const QString&) ));

  /* **************************************************************
   *
   *  minimum value text entry
   *
   * ************************************************************** */
     
  new QLabel ("Min:", hbox);
  minimum_value = new QLineEdit (hbox);

  connect (minimum_value, SIGNAL(lostFocus()),
	   this, SLOT(minimum_value_CB()));
  connect (minimum_value, SIGNAL(returnPressed()),
	   this, SLOT(mimimum_value_CB()));
  connect (minimum_value, SIGNAL( textChanged(const QString&) ),
	   this, SLOT( text_changed_CB(const QString&) ));

  /* **************************************************************
   *
   *  maximum value text entry
   *
   * ************************************************************** */

  new QLabel ("Max:", hbox);
  maximum_value = new QLineEdit (hbox);

  connect (maximum_value, SIGNAL(lostFocus()),
	   this, SLOT(maximum_value_CB()));
  connect (maximum_value, SIGNAL(returnPressed()),
	   this, SLOT(maximum_value_CB()));
  connect (maximum_value, SIGNAL( textChanged(const QString&) ),
	   this, SLOT( text_changed_CB(const QString&) ));

  /* **************************************************************
   *
   *  parameter type drop down
   *
   * ************************************************************** */

  new QLabel ("Type:", hbox);
  conversion_type = new QComboBox (hbox);

  conversion_type->insertItem ("integer");
  conversion_type->insertItem ("real");

  connect (conversion_type, SIGNAL( highlighted(const QString&) ),
	   this, SLOT( conversion_type_CB(const QString&) ));

  /* **************************************************************
   *
   *  scroll bar
   *
   * ************************************************************** */

  scroll_bar = new QScrollBar (this);
  scroll_bar->setOrientation (Qt::Horizontal);

  connect (scroll_bar, SIGNAL( valueChanged(int) ),
	   this, SLOT( scroll_bar_CB(int) ));

  range_policy = new IntegerRangePolicy( scroll_bar->minValue(),
					 scroll_bar->maxValue() );

  minimum_value->setText( range_policy->get_output_min().c_str() );
  maximum_value->setText( range_policy->get_output_max().c_str() );

  setFocusPolicy (QWidget::StrongFocus);
}

void ConfigurableScrollBar::text_changed_CB (const QString& text)
{
  if (parameter_name->isModified())
    parameter_name->setPalette (highlight_palette);

  if (minimum_value->isModified())
    minimum_value->setPalette (highlight_palette);

  if (maximum_value->isModified())
    maximum_value->setPalette (highlight_palette);

  scroll_bar->setEnabled (false);
}

void ConfigurableScrollBar::parameter_name_CB ()
{
  parameter_name->clearModified ();
  parameter_name->setPalette (default_palette);
  scroll_bar->setEnabled (true);

  current_parameter_name = parameter_name->text().ascii();
}

void ConfigurableScrollBar::minimum_value_CB ()
{
  minimum_value->setPalette (default_palette);

  range_policy->set_output_min( minimum_value->text().ascii() );
  minimum_value->setText( range_policy->get_output_min().c_str() );

  scroll_bar->setMinValue( range_policy->get_input_min() );
  scroll_bar->setEnabled (true);
}

void ConfigurableScrollBar::maximum_value_CB ()
{
  maximum_value->setPalette (default_palette);

  range_policy->set_output_max( maximum_value->text().ascii() );
  maximum_value->setText( range_policy->get_output_max().c_str() );

  scroll_bar->setMaxValue( range_policy->get_input_max() );
  scroll_bar->setEnabled (true);
}

void ConfigurableScrollBar::conversion_type_CB (const QString& selected)
{
  if (selected == "real")
    set_range_policy( new RealRangePolicy );
  else
    set_range_policy( new IntegerRangePolicy( scroll_bar->minValue(),
					      scroll_bar->maxValue() ) );
}

void ConfigurableScrollBar::set_range_policy (RangePolicy* policy)
{
  if (range_policy)
  {
    policy->set_output_min( range_policy->get_output_min() );
    policy->set_output_max( range_policy->get_output_max() );
    delete range_policy;
  }

  range_policy = policy;

  minimum_value->setText( range_policy->get_output_min().c_str() );
  maximum_value->setText( range_policy->get_output_max().c_str() );

  scroll_bar->setMinValue( range_policy->get_input_min() );
  scroll_bar->setMaxValue( range_policy->get_input_max() );
}

void ConfigurableScrollBar::scroll_bar_CB (int value)
{
  if (current_parameter_name != "")
    output (current_parameter_name + "=" + range_policy->get_output (value));
}

