<?php

function artwork_overview_types() {
  foreach (artwork_types() as $type => $info) {
    $type_url_str = str_replace('_', '-', $type);
    $label = t('View @type', array('@type' => $info->name));

    //variables declared in a loop in php are persisted across iterations.
    // $items[] increments the array key at each iteration automatically, ie $items[0], $items[1], $items[2]...
    $items[] = l($label, 'admin/structure/artworks/manage/' . $type_url_str);
  }

  return theme('item_list', array('items' => $items));
}

function artwork_type_page_title($type) {
  return t('Manage @type', array('@type' => $type->name));
}

function artwork_information($artwork_type) {
  return $artwork_type->name . ': ' . $artwork_type->description;
}