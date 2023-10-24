\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{homework}[2019/09/09 Kevin's Homework class]
\LoadClass[11pt, letterpaper]{article}

\RequirePackage[utf8]{inputenc}
\RequirePackage[margin=1in]{geometry}
\RequirePackage{fancyhdr}
\RequirePackage{enumitem}
\RequirePackage{amsmath}

\setlist[enumerate,1]{label=(\alph*)}

\newcommand{\setclass}[2]{\newcommand{\classname}{#1} \newcommand{\classnumber}{#2}}
\newcommand{\studentid}[1]{\newcommand{\authorid}{#1}}
\pagestyle{fancy}
\fancyhf{}
\fancyhead[LE,LO]{\@title}
\fancyhead[RE,RO]{\@author\space(\authorid)}
\fancyfoot[CE,CO]{\thepage}


\newcommand{\bvec}[1]{ \boldsymbol{\mathrm{#1}} }
\newcommand{\uvec}[1]{  \thinspace\hat{ \mathbf{a}}_{#1} }

\renewcommand{\maketitle}{
	\thispagestyle{empty}
	{\noindent\fontsize{20}{30}\selectfont\@title}
	\vspace{0.5em}
	
	\noindent\begin{minipage}{0.5\linewidth}
		\noindent\ifdefined\classname
\ifdefined\classnumber
\textsc{\classname} \classnumber
\fi
\fi

\noindent\@date
	\end{minipage}
	\begin{minipage}{0.5\linewidth}

\flushright \@author

\ifdefined\authorid
ID: \authorid
\fi	
	\end{minipage}
	\begin{center}
		\noindent\rule{1\linewidth}{0.5pt}
	\end{center}
}
\endinput
