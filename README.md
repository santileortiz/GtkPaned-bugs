GtkPaned Bugs
=============

So after attempting to use GtkPaned on several of my applications I'va came
accross a lot of bugs. Here I'm puting together all the minimal examples I've
been able to put together to show the issues.

To get an understanding of how the bugs relate, or how I've come to find them,
and also make them fail just as I did, study them in elementary OS and in the
following order:

  1. size\_allocation\_warning.c
  2. separator\_vanishes\_fix.c
  3. useless\_set\_pos\_percentage.c
  4. child\_does\_not\_show.c

In total I think I've found 4 bugs. For the first 2 I have fixes with which I
can work around. For the first one I also have a patch for Gtk that would fix
it. The second one I think I manage to work around by guessing, or I don't
remember how it occured to me (I don't have a patch). The last two still bother
me but I don't have more energy to try to dig deeper, I just decided to stop
using GtkPaned.

I may write a blog soon narrating how I found them and explaining more details.
If you find this, a lot of time has passed and the bugs are still there,
chances are I won't even know much about this and your best bet is to take this
code and study what is going on.

