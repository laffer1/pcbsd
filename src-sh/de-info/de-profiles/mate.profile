DE_NAME="Mate"
XDG="YES"
DE_SU="pc-su %s"
DE_FILEMAN="nautilus %s"
DE_TERMINAL="gnome-terminal"
DE_TERMINAL_TITLE="-t"
DE_TERMINAL_COMMAND="-e"
DE_SESSION_NAME="MATE"

active() {
if [ "$PCDM_SESSION" = "$DE_SESSION_NAME" ]; then
DE_ACTIVE=1
fi  

}

installed() {
# Check via pc-metapkgmanager if it exists
which pc-metapkgmanager >/dev/null 2>/dev/null
if [ $? -eq 0 ] ; then
  pc-metapkgmanager status Mate | grep "is installed" >/dev/null
  if [ $? -eq 0 ] ; then
    DE_INSTALLED=1
  fi
else
  which mate-session >/dev/null 2>/dev/null
  if [ $? -eq 0 ] ; then
    DE_INSTALLED=1
  fi
fi

}
