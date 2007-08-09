

trigger:=[=];

  triggergui:=[=];
  triggergui.telnet:=F;

trigger.constructor:=function(){
  global triggergui;

  triggergui:=[=];
  triggergui.telnet:=F;

  triggergui.logfile:=open(">>trigger-log.log");
  triggergui.frame := frame(title=spaste('Trigger for ITS'));
  triggergui.frame1 := frame(triggergui.frame,relief='ridge',padx=50,pady=10,height=100,width=60);

  triggergui.frametrig := frame(triggergui.frame1,side='left');
  triggergui.trigopen := button(triggergui.frametrig, 'Open Trigger', background='white', foreground='black');
  triggergui.frametrig_sp1 :=frame(triggergui.frametrig, height=1, width=5, expand='x');
  triggergui.trigbut := button(triggergui.frametrig, 'Send Trigger', background='green', foreground='white');
  triggergui.frametrig_sp2 :=frame(triggergui.frametrig, height=1, width=5, expand='x');
  triggergui.trigstat := button(triggergui.frametrig, 'Status of Trigger', background='white', foreground='black');

  triggergui.commentbut := button(triggergui.frame1, 'Save comments for logfile', foreground='white');

   triggergui.ftext:=frame(triggergui.frame1);
    tf := frame(triggergui.ftext,side='left',borderwidth=0)
    t := text(tf,relief='sunken',wrap='word',background='white')
    vsb := scrollbar(tf)
#    bf := frame(triggergui.frame1,side='right',borderwidth=0)
#    pad := frame(bf,expand='none',width=60,height=5,relief='groove')
#    hsb := scrollbar(bf,orient='horizontal')
#    whenever vsb->scroll, hsb->scroll do
    whenever vsb->scroll do
        t->view($value)
    whenever t->yscroll do
        vsb->view($value)
#    whenever t->xscroll do
#        hsb->view($value)

  triggergui.chatframe := frame(triggergui.frame1,relief='ridge',height=100,width=60);

  triggergui.chatlineframe := frame(triggergui.chatframe,side='left',borderwidth=0);
  triggergui.chatlabel := label(triggergui.chatlineframe, 'Chat:', justify='left', relief='flat');
  triggergui.chatline := entry(triggergui.chatlineframe,relief='sunken',width=50,background='white');

   triggergui.chat_text:=frame(triggergui.chatframe);
    triggergui.chat_tf := frame(triggergui.chat_text,side='left',borderwidth=0)
    triggergui.chat_t := text(triggergui.chat_tf,relief='sunken',wrap='word',background='white')
    triggergui.chat_vsb := scrollbar(triggergui.chat_tf)
#    triggergui.chat_bf := frame(triggergui.chatframe,side='right',borderwidth=0)
#    triggergui.chat_pad := frame(triggergui.chat_bf,expand='none',width=60,height=5,relief='groove')
#    triggergui.chat_hsb := scrollbar(triggergui.chat_bf,orient='horizontal')
#    whenever triggergui.chat_vsb->scroll, triggergui.chat_hsb->scroll do
    whenever triggergui.chat_vsb->scroll do
        triggergui.chat_t->view($value)
    whenever triggergui.chat_t->yscroll do
        triggergui.chat_vsb->view($value)
#    whenever triggergui.chat_t->xscroll do
#        triggergui.chat_hsb->view($value)


  triggergui.frame2 := frame(triggergui.frame,relief='ridge',padx=50,pady=10,height=100,width=60,side='left');
  triggergui.dismissbut := button(triggergui.frame2, 'Dismiss', background='yellow', foreground='black');
  triggergui.frame2sp :=frame(triggergui.frame2, height=1, width=20, expand='x')
  triggergui.killbut := button(triggergui.frame2, 'Kill', background='red', foreground='white');
  triggergui.frame3 := frame(triggergui.frame,relief='ridge',padx=50,pady=10,height=100,width=60);
  triggergui.messlabel := label(triggergui.frame3, '', justify='left', relief='flat', borderwidth=0,width=60);

whenever triggergui.trigopen->press do {
 triggergui.telnet:=F;
 shell('rm -f $LOPESCODEDIR/trigger-telnet.log');
 triggergui.telnet:=open('|csh -c "telnet astron-buinen.nema.rug.nl 2222 |& tee $LOPESCODEDIR/trigger-telnet.log"');
# triggergui.telnet:=open('| csh -c "telnet 10.87.2.71 |& tee $LOPESCODEDIR/trigger-telnet.log"');
 j:=0; for (i in seq(400000)) {j+:=1};
 line := shell('tail $LOPESCODEDIR/trigger-telnet.log') ~ s/\r/\n/g;
 triggergui.chat_t->append(line);
}


whenever triggergui.trigbut->press do {
   triggergui.date:=shell('date');
   write(triggergui.telnet,'\n');
   write(triggergui.telnet,'exit\n');
   print "\nTrigger pressed at ",triggergui.date
   write(triggergui.logfile,' ','#Trigger:',spaste("Trigger pressed at ",triggergui.date));
   triggergui.messlabel->text(spaste('Trigger has been sent at ',triggergui.date,'!'));
   line := shell('tail -3 $LOPESCODEDIR/trigger-telnet.log') ~ s/\r/\n/g;
   triggergui.chat_t->append(line);
   triggergui.telnet:=F;
};

whenever triggergui.trigstat->press do {
   line := shell('tail $LOPESCODEDIR/trigger-telnet.log') ~ s/\r/\n/g;
   print "#Status:";
   print line;
   triggergui.chat_t->append(line);
};


whenever triggergui.commentbut->press do {
   lines:= t-> get("start","end");
   print "#Comment:"
   print lines
   write(triggergui.logfile,'#Comment:',lines);
   triggergui.messlabel->text(spaste('Comment sent!'));
};

whenever triggergui.chatline->return do {
  line:=spaste('\n#Remote User: ',triggergui.chatline->get("start","end"));
  print line;
  triggergui.chat_t->append(line);
  triggergui.chatline->delete("start","end");
}

whenever triggergui.dismissbut->press do {
   print "Window Dismissed";
   val triggergui.frame :=F
   val triggergui:=F;
};

whenever triggergui.killbut->press do {
   print "Kill"
   exit;
};

}

trigger.gui:=trigger.constructor

trigger.chat:=function(){ 
  local line; global triggergui
  print 'Type "exit" to leave chat, type text<enter> to send chat line.'
  print 'Type "dismiss" to delete remote trigger gui or "kill" to exit alltogether!' 
  line:='start';
  while (line != "exit") {
    line:=readline(prompt='#ITS Console: ');
      triggergui.chat_t->append(spaste('\n#ITS Console: ',line))
    if (line == "dismiss") {triggergui.frame:=F; line:="exit"};
    if (line == "kill") {exit};
  };
}



