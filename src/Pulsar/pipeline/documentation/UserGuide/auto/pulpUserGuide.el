(TeX-add-style-hook "pulpUserGuide"
 (lambda ()
    (LaTeX-add-labels
     "sec:introduction"
     "sec:package"
     "tab:recipeTable"
     "sec:environment"
     "sec:config"
     "sec:cfgfiles"
     "sec:interfaces"
     "sec:discussion"
     "sec:open-questions"
     "fig:singlenode"
     "fig:fpppic"
     "sec:future-enhancements")
    (TeX-run-style-hooks
     "svninfo"
     "today"
     "nofancy"
     "supertabular"
     "usg"
     "a4wide"
     "latex2e"
     "scrartcl10"
     "scrartcl"
     "a4paper"
     "10pt"
     "bibtotoc")))

