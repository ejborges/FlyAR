---
layout: page
title: FlyAR
tagline: Augmented Reality for the Sky
---
{% include JB/setup %}
    
## Project Updates

Below are all of the project updates so far. Click on a specific update to read more about the update.

    $ rm -rf _posts/core-samples

Here's a sample "posts list".

<ul class="posts">
  {% for post in site.posts %}
    <li><span>{{ post.date | date_to_string }}</span> &raquo; <a href="{{ BASE_PATH }}{{ post.url }}">{{ post.title }}</a></li>
  {% endfor %}
</ul>