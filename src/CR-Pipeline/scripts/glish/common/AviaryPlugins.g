
#-------------------------------------------------------------------------------
# LOPES Tools interface to the plugins usable with the AVIARY control software
# of the LOFAR ITS.
#-------------------------------------------------------------------------------



#-------------------------------------------------------------------------------
#Section: Basic data structures and definition of plugin options
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: getPlugin
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Provide access to the plugin structure of the AVIARY control software.
#      The name, parameters, etc. of the available plugins are packed into a
#      record, which then can be accessed using this method.
#Par:  index = 'all' - Indicate the plugin on which we want to receive
#                      information; supported options are:
#                      <ol>
#                       <li><tt>index='all'</tt>: retrieve the complete record
#                           for all available plugins.
#                       <li><tt>index='&lt;name&gt;'</tt>: retrieve information
#                           of a specific plugin
#                       <li><tt>index='local'</tt>: retrieve a record with the 
#                           plugins running locally on the capture PCs
#                       <li><tt>index='central'</tt>:  retrieve a record with
#                           the  plugins running on the central server
#                      </ol>
#Ret:  recPlugins - Record with the list of available plugins and their 
#      parameters.
#-------------------------------------------------------------------------------

getPlugin := function (index='all')
{
  plugins := [=];

  #---------------------------------------------------------------------
  # No Plugin
  #
  #  The nopPlugin is just copying data from in- to out-put. If an
  #  input exists without output, the data is discarded

  plugins['nop'] := [=];
  plugins['nop'].doc := 'No Plugin (data pass-through)';
  plugins['nop'].location := 'all';

  #---------------------------------------------------------------------
  # Beamformer                                           [BF/bfPlugin.h]
  #
  #  The beam former plugin is a module which processes complex input
  #  data by multiplying it using a weight factor and adding the outcome.
  #  Optionally the result can be integrated over a defined period.

  plugins['bf'] := [=];
  plugins['bf'].doc := 'Beamformer';
  plugins['bf'].location := 'central';
  plugins['bf'].options['-c'].doc    := 'Frequency and number of channel groups';
  plugins['bf'].options['-c'].value  := '1000:10,25e3:2';
  plugins['bf'].options['-c'].syntax := "f:n[,f:n]";
  plugins['bf'].options['-d'].doc    := 'Frequency difference between channels';
  plugins['bf'].options['-d'].value  := '1e03';
  plugins['bf'].options['-d'].syntax := "DOUBLE";
  plugins['bf'].options['-u'].doc    := 'Grid of l coordinates';
  plugins['bf'].options['-u'].value  := '-1:0.05:1';
  plugins['bf'].options['-u'].syntax := "start:delta:end";
  plugins['bf'].options['-v'].doc    := 'Grid of m coordinates';
  plugins['bf'].options['-v'].value  := '-1:0.05:1';
  plugins['bf'].options['-v'].syntax := "start:delta:end";
  plugins['bf'].options['-i'].doc    := 'Number of integration before creating output data';
  plugins['bf'].options['-i'].value := 1;
  plugins['bf'].options['-i'].syntax := "INT";
  plugins['bf'].options['-a'].doc   := 'Antenna positions file';
  plugins['bf'].options['-a'].value := 'antPos.dat';
  plugins['bf'].options['-a'].syntax := "STRING";

  #---------------------------------------------------------------------
  # Correlator                           [Correlator/correlatorPlugin.h]
  #
  #  The correlator plugin is a module which produces a correlation
  #  matrix of provided complex data.

  plugins['correlator'] := [=];
  plugins['correlator'].doc := 'Correlator';
  plugins['correlator'].location := 'central';
  plugins['correlator'].options['-c'].doc    := 'number of channels put into correlator';
  plugins['correlator'].options['-c'].value  := '4096';
  plugins['correlator'].options['-c'].syntax := 'INT';
  plugins['correlator'].options['-i'].doc    := 'number of output channels';
  plugins['correlator'].options['-i'].value  := '2048';
  plugins['correlator'].options['-i'].syntax := 'INT';

  #---------------------------------------------------------------------
  # Complex polyphase filter                         [opf/qcopfPlugin.h]
  #
  #  The copfPlugin is a module which implements a complex (oversampled)
  #  polyphase filter. The structure of this filter is defined by the   
  #  content of the coefficients file. 

  plugins['copf'] := [=];
  plugins['copf'].doc := 'Complex polyphase filter';
  plugins['copf'].location := 'all';
  plugins['copf'].options['-c'].doc    := 'Name of filter coefficients file.';
  plugins['copf'].options['-c'].value  := '';
  plugins['copf'].options['-c'].syntax := 'STRING';
  plugins['copf'].options['-N'].doc    := 'Decimation factor (N).';
  plugins['copf'].options['-N'].value  := '';
  plugins['copf'].options['-N'].syntax := 'INT';
  plugins['copf'].options['-e'].doc    := 'Enable channels.';
  plugins['copf'].options['-e'].value  := '';
  plugins['copf'].options['-e'].syntax := 'chA:chB[,chC:chD]';

  #-------------------------------------------------------------------
  # N-point Fourier Transform                       [FFT/fftPlugin.cc]
  #
  #  The fftPlugin is a module which implement an N-point FFT.
  #  Optionally Hanning windowing can be configured.

  plugins['fft'] := [=];
  plugins['fft'].doc := 'Fourier Transform';
  plugins['fft'].location := 'all';
  plugins['fft'].options['-c'].doc    := 'Number of points (channels) in the fft.';
  plugins['fft'].options['-c'].value  := '';
  plugins['fft'].options['-c'].syntax := 'INT';
  plugins['fft'].options['-e'].doc    := 'Enable channels.';
  plugins['fft'].options['-e'].value  := '';
  plugins['fft'].options['-e'].syntax := 'chA:chB[,chC:chD]';
  plugins['fft'].options['-d'].doc    := 'Data aquisition board used as input.';
  plugins['fft'].options['-d'].value  := '';
  plugins['fft'].options['-d'].syntax := 'BOOL';
  plugins['fft'].options['-w'].doc    := 'Windowing type (default no windowing).';
  plugins['fft'].options['-w'].value  := '';
  plugins['fft'].options['-w'].syntax := 'STRING';

  #-------------------------------------------------------------------
  # FIR filter                                       [FIR/firPlugin.h]
  #
  #  The firPlugin is a module which implement an N-taps FIR filter

  plugins['fir'] := [=];
  plugins['fir'].doc := 'FIR filter';
  plugins['fir'].location := 'all';
  plugins['fir'].options['-c'].doc    := 'Name of filter coefficients file';
  plugins['fir'].options['-c'].value  := '';
  plugins['fir'].options['-c'].syntax := 'STRING';
  plugins['fir'].options['-d'].doc    := 'Data aquisition used as input';
  plugins['fir'].options['-d'].value  := '';
  plugins['fir'].options['-d'].syntax := 'BOOL';

  #-------------------------------------------------------------------
  # Oversampled polyphase filter                     [opf/opfPlugin.h]
  #
  #  The opfPlugin is a module which implements a (oversampled) polyphase
  #  filter. The structure of this filter is defined by the content of   
  #  the coefficients file.

  plugins['opf'] := [=];
  plugins['opf'].doc := 'Oversampled polyphase filter';
  plugins['opf'].location := 'all';
  plugins['opf'].options['-c'].doc    := 'Name of filter coefficients file.';
  plugins['opf'].options['-c'].value  := '';
  plugins['opf'].options['-c'].syntax := 'STRING';
  plugins['opf'].options['-d'].doc    := 'Data aquisition used as input.';
  plugins['opf'].options['-d'].value  := '';
  plugins['opf'].options['-d'].syntax := 'BOOL';
  plugins['opf'].options['-N'].doc    := 'Decimation factor (N).';
  plugins['opf'].options['-N'].value  := '';
  plugins['opf'].options['-N'].syntax := 'INT';
  plugins['opf'].options['-e'].doc    := 'Enable channels.';
  plugins['opf'].options['-e'].value  := '';
  plugins['opf'].options['-e'].syntax := 'chA:chB[,chC:chD]';

  #-------------------------------------------------------------------
  # Polyphase filter                                 [ppf/ppfPlugin.h]
  #
  #  The ppfPlugin is a module which implements a polyphase filter. The    
  #  structure of this filter is defined by the content of the
  #  coefficients file.

  plugins['ppf'] := [=];
  plugins['ppf'].doc := 'Polyphase filter';
  plugins['ppf'].location := 'all';
  plugins['ppf'].options['-c'].doc    := 'Name of filter coefficients file';
  plugins['ppf'].options['-c'].value  := '';
  plugins['ppf'].options['-c'].syntax := 'STRING';
  plugins['ppf'].options['-d'].doc    := 'Data aquisition used as input';
  plugins['ppf'].options['-d'].value  := '';
  plugins['ppf'].options['-d'].syntax := 'BOOL';

  #------------------------------------------------------------------
  # Quantized beam former                            [BF/qbfPlugin.h]
  #
  #  The quantized beam former plugin is a module which processes
  #  complex input data (i.e. from quantized polyphase filter) by
  #  multiplying it using a weight factor and adding the outcome.

  plugins['qbf'] := [=];
  plugins['qbf'].doc := 'Quantized beam former';
  plugins['qbf'].location := 'central';
  plugins['qbf'].options['-c'].doc   := 'Frequency and number of channel groups';
  plugins['qbf'].options['-c'].value := '1000:10,25e3:2';
  plugins['qbf'].options['-c'].value := 'f:n[,f:n]';
  plugins['qbf'].options['-d'].doc   := 'Frequency difference between channels';
  plugins['qbf'].options['-d'].value := '1e03';
  plugins['qbf'].options['-d'].value := 'DOUBLE';
  plugins['qbf'].options['-u'].doc   := 'Grid of l coordinates';
  plugins['qbf'].options['-u'].value := '-1:0.05:1';
  plugins['qbf'].options['-u'].value := 'start:delta:end';
  plugins['qbf'].options['-v'].doc   := 'Grid of m coordinates';
  plugins['qbf'].options['-v'].value := '-1:0.05:1';
  plugins['qbf'].options['-v'].value := 'start:delta:end';
  plugins['qbf'].options['-a'].doc   := 'Antenna positions file';
  plugins['qbf'].options['-a'].value := 'antPos.dat';
  plugins['qbf'].options['-a'].value := 'STRING';

  #-------------------------------------------------------------------
  # Quantized complex oversampled polyphase filter [opf/qcopfPlugin.h]
  #
  #  The qcopfPlugin is a module which implements a quantized complex     
  #  (oversampled) polyphase filter. The only right input data is 32 bits 
  #  signed. If a critically sampled filter is required, then set the     
  #  decimation factor equal to K. The structure of this filter is defined
  #  by the content of the coefficients file.

  plugins['qcopf'] := [=];
  plugins['qcopf'].doc := 'Quantized complex polyphase filter';
  plugins['qcopf'].location := 'all';
  plugins['qcopf'].options['-c'].doc    := 'Name of filter coefficients file.';
  plugins['qcopf'].options['-c'].value  := '';
  plugins['qcopf'].options['-c'].syntax := 'STRING';
  plugins['qcopf'].options['-N'].doc    := 'Decimation factor (N).';
  plugins['qcopf'].options['-N'].value  := '';
  plugins['qcopf'].options['-N'].syntax := 'INT';
  plugins['qcopf'].options['-e'].doc    := 'Enable channels.';
  plugins['qcopf'].options['-e'].value  := '';
  plugins['qcopf'].options['-e'].syntax := 'chA:chB[,chC:chD]';

  #-------------------------------------------------------------------
  # Quantized oversampled polyphase filter          [opf/qopfPlugin.h]
  #
  #  The qopfPlugin is a module which implements a quantized (oversampled) 
  #  polyphase filter. The only right input data is 16 bits signed. The    
  #  structure of this filter is defined by the content of the coefficients
  #  file.

  plugins['copf'] := [=];
  plugins['copf'].doc := 'Quantized polyphase filter';
  plugins['copf'].location := 'all';
  plugins['copf'].options['-c'].doc    := 'Name of filter coefficients file.';
  plugins['copf'].options['-c'].value  := '';
  plugins['copf'].options['-c'].syntax := 'STRING';
  plugins['copf'].options['-N'].doc    := 'Decimation factor (N).';
  plugins['copf'].options['-N'].value  := '';
  plugins['copf'].options['-N'].syntax := 'INT';
  plugins['copf'].options['-e'].doc    := 'Enable channels.';
  plugins['copf'].options['-e'].value  := '';
  plugins['copf'].options['-e'].syntax := 'chA:chB[,chC:chD]';

  #-------------------------------------------------------------------
  # Plot of frequency spectra              [Spectrum/spectrumPlugin.h]
  #
  #  The spectrumPlugin is a module that collects frequency
  #  spectra from all active antenna and plots these to the
  #  screen in a surface plot.

  plugins['spectrum'] := [=];
  plugins['spectrum'].doc := 'Plot of frequency spectra';
  plugins['spectrum'].location := 'central';
  plugins['spectrum'].options['-d'].doc    := 'display to open figure at';
  plugins['spectrum'].options['-d'].value  := '';
  plugins['spectrum'].options['-d'].syntax := 'STRING';
  plugins['spectrum'].options['-c'].doc    := 'number of frequency channels';
  plugins['spectrum'].options['-c'].value  := '';
  plugins['spectrum'].options['-c'].syntax := 'INT';
  plugins['spectrum'].options['-i'].doc    := 'number of iterations to average over';
  plugins['spectrum'].options['-i'].value  := '';
  plugins['spectrum'].options['-i'].syntax := 'INT';
  plugins['spectrum'].options['-p'].doc    := 'interval between plots in samples';
  plugins['spectrum'].options['-p'].value  := '';
  plugins['spectrum'].options['-p'].syntax := 'INT';

  #-------------------------------------------------------------------
  # Process the information request
  
  if (index == 'all') return plugins;
  else if (index == 'local') {
    recPlugins := [=];
    # get a list of all available plugins (i.e. their names)
    pluginsAll := field_names(getPlugin(index='all'));
    # go through the list and extract the plugins running locally
    for (i in pluginsAll) {
      if (plugins[i].location != 'central') {
        recPlugins[i] := plugins[i];
      }
    }
    return recPlugins;
  }
  else if (index == 'central') {
    recPlugins := [=];
    # get a list of all available plugins (i.e. their names)
    pluginsAll := field_names(getPlugin(index='all'));
    # go through the list and extract the plugins running centrally
    for (i in pluginsAll) {
      if (plugins[i].location != 'local') {
        recPlugins[i] := plugins[i];
      }
    }
    return recPlugins;
  }
  else return plugins[index];

}


#-------------------------------------------------------------------------------
#Name: listPlugins
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Print a summary of the available plugins.
#-------------------------------------------------------------------------------

listPlugins := function ()
{
  recPlugins := getPlugin(index='all');
  plugins := field_names(recPlugins);
  
  for (i in plugins) {
    options := field_names(recPlugins[i].options);
    print "[",i,"]";
    print "++ Description :",recPlugins[i].doc;
    print "++ Location .. :",recPlugins[i].location;
    print "++ Options ... :",options;
    print "";
  }
  
}


#-------------------------------------------------------------------------------
#Section: Graphical interface to the plugin options
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: setPluginParameters
#-------------------------------------------------------------------------------
#Type: Glish Function
#Doc:  Provide a graphical interface to set up the options for a Plugin; this
#      function creates a single stand-alone GUI which can be used to 
#      set the values for the options of plugin.
#Par:  recGUI - record holding data of the parent GUI.
#Created: 2004/07/15 (Lars Baehren)
#-------------------------------------------------------------------------------

setPluginParameters := function (ref recGUI)
{
  # Get all relevant information on the selected plugin
  plugin := recGUI.data;
  recPlugin := getPlugin(index=plugin);

  global plugingui;

  plugingui.panel := frame(title=spaste('AVIARY Plugin setup'));
  plugingui.frame := frame(plugingui.panel,relief='ridge',width=100);

  #-------------------------------------------------------------------
  # Headline indicating plugin name

  labeltext := spaste(plugin,' : ',recPlugin.doc);

  plugingui.label := label (plugingui.frame,text=labeltext,
                            background='grey70',foreground='blue',fill='x');

  #-------------------------------------------------------------------
  # Entry fields for the plugin options

  options := field_names(recPlugin.options);

  for (opt in options) {
    plugingui.option[opt].f := frame (plugingui.frame);
    plugingui.option[opt].l := label (plugingui.option[opt].f,
                                      text=as_string(recPlugin.options[opt].doc),
                                      fill='x',anchor='w');
    plugingui.option[opt].e := entry (plugingui.option[opt].f,
                                      background='white',
                                      relief='sunken');
    plugingui.option[opt].e->insert(as_string(recPlugin.options[opt].syntax));
  }

  #-------------------------------------------------------------------
  # Control buttons of the GUI and handling of the related events

  plugingui.control := frame(plugingui.frame,side='left',expand='x',relief='groove');
  plugingui.accept  := button(plugingui.control,'Accept',background='green',padx=5);
  plugingui.dismiss := button(plugingui.control,'Dismiss',background='orange',padx=5);

  # Write the entered parameter values back to the plugin data record

  whenever plugingui.accept->press do {
    optionsString := spaste(plugin,'Plugin');
    for (opt in options) {
      recPlugin.options[opt].value := as_string(plugingui.option[opt].e->get());
      optionsString := spaste (optionsString,' ',opt,' ',recPlugin.options[opt].value);
    }
    # update the contents of the entry field
    recGUI.opt->delete('start','end');
    recGUI.opt->insert(optionsString);
    # update the value of the plugin data variable (which is read out later)
    recGUI.data := as_string(optionsString);
    val plugingui.panel := F;
  }

  # Dismiss the GUI without further other actions

  whenever plugingui.dismiss->press do {
    val plugingui.panel := F; 
  }

}