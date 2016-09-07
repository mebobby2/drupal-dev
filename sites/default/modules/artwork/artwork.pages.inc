<?php

function artwork_add_page() {
  $item = menu_get_item(); //gets the current menu
  $links = system_admin_menu_block($item); //gets the children of an arbitrary menu item

  foreach ($links as $link) {
    $items[] = l($link['title'], $link['href'],
                 $item['localized_options'])
                 . ': ' . filter_xss_admin($link['description']);
  }

  return theme('item_list', array('items' => $items));
}

function artwork_add($type) {
  global $user;

  $types = artwork_types();
  $type = isset($type) ? str_replace('-', '_', $type) : NULL;
  if (empty($types[$type])) {
    return MENU_NOT_FOUND;
  }

  $artwork = entity_get_controller('artwork')->create($type);

  drupal_set_title(t('Create @name', array('@name' => $types[$type]->name)), PASS_THROUGH);
  return drupal_get_form($type . '_artwork_form', $artwork);
}

function artwork_form($form, &$form_state, $artwork) {
  $form['#id'] = 'artwork-form';

  $form['#artwork'] = $artwork;
  $form_state['artwork'] = $artwork;

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $artwork->title,
    '#weight' => -5,
    '#required' => TRUE,
  );

  $form['revision'] = array(
    '#access' => user_access('administer artworks'),
    '#type' => 'checkbox',
    '#title' => t('Create new revision'),
    '#default_value' => 0,
  );

  $form['buttons'] = array();
  $form['buttons']['#weight'] = 100;
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
    '#submit' => array('artwork_form_submit'),
  );

  if (!empty($artwork->aid)) {
    $form['buttons']['delete'] = array(
     '#access' => user_access('delete artworks'),
     '#type' => 'submit',
     '#value' => t('Delete'),
     '#weight' => 15,
     '#submit' => array('artwork_form_delete_submit'),
    );
  }

  $form['#validate'][] = 'artwork_form_validate';

  field_attach_form('artwork', $artwork, $form, $form_state);

  return $form;
}

function artwork_form_validate($form, &$form_state) {
  $artwork = $form_state['artwork'];

  field_attach_form_validate('artwork', $artwork, $form, $form_state);
}

function artwork_form_submit($form, &$form_state) {
  global $user; //$user is like the current_user. It's stored globally.

  $artwork = &$form_state['artwork'];

  if (empty($artwork->uid)) {
    $artwork->uid = $user->uid;
  }

  $artwork->title = $form_state['values']['title'];
  $artwork->revision = $form_state['values']['revision'];

  field_attach_submit('artwork', $artwork, $form, $form_state);

  artwork_save($artwork);

  drupal_set_message(t('Artwork saved.'));

  $form_state['redirect'] = 'artwork/' . $artwork->aid;
}

function artwork_save($artwork) {
  return entity_get_controller('artwork')->save($artwork);
}

function artwork_page_title($artwork) {
  return $artwork->title;
}

function artworks_page() {
  $result = artwork_list_recent(10000);
  $artworks = artwork_load_multiple(array_keys($result['artwork']));
  $artwork_links = array();
  foreach ($artworks as $artwork) {
    $artwork_links[] = l($artwork->title, 'artwork/' . $artwork->aid . '/edit', array());
  }
  return array(
    '#type' => 'container',
    'artwork' => array(
      '#theme' => 'item_list',
      '#items' => $artwork_links,
    ),
    'view_all' => array(
      '#type'  => 'link',
      '#title' => 'Add new',
      '#href'  => 'artwork/add',
    ),
  );
}

function artwork_page_edit($artwork) {
  $types = artwork_types();
  drupal_set_title(
    t('<em>Edit @type</em> @title', 
      array('@type' => $types[$artwork->type]->name,
          '@title' => $artwork->title)),
    PASS_THROUGH
  );

  return drupal_get_form($artwork->type . '_artwork_form', $artwork);
}

function artwork_page_view($artwork, $view_mode = 'full') {
  $artwork->content = array();
  if ($view_mode == 'teaser') {
    $artwork->content['title'] = array(
      '#markup' => filter_xss($artwork->title),
      '#weight' => -5,
    );
  }

  field_attach_prepare_view('artwork', array($artwork->aid => $artwork), $view_mode);

  entity_prepare_view('artwork', array($artwork->aid => $artwork));
  $artwork->content += field_attach_view('artwork', $artwork, $view_mode);

  return $artwork->content;
}

function artwork_page_list_recent($list_count = 5) {
  $content = array();
  $result = artwork_list_recent($list_count);

  $artworks = artwork_load_multiple(array_keys($result['artwork']));
  foreach ($artworks as $artwork) {
    $content[$artwork->aid] = artwork_page_view($artwork, 'teaser');
  }

  return $content;
}