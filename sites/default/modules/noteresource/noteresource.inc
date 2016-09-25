<?php

function _noteresource_create($data) {
  global $user;

  unset($data->id);
  $data->uid = $user->uid;
  $data->created = time();
  $data->modified = time();

  if (!isset($data->subject)) {
    return services_error('Missing note attribute subject', 406);
  }

  if (!isset($data->note)) {
    return services_error('Missing note attribute note', 406);
  }

  noteresource_write_note($data);
  return (object)array(
    'id' => $data->id,
    'uri' => services_resource_uri(array('note', $data->id)),
  );
}

function _noteresource_update($id, $data) {
  global $user;
  $note = noteresource_get_note($id);

  unset($data->created);
  $data->id = $id;
  $data->uid = $note->uid;
  $data->modified = $time();

  noteresource_write_note($data);
  return (object)array(
    'id' => $id,
    'uri' => services_resource_uri(array('note', $id)),
  );
}

function _noteresource_retrieve($id) {
  return noteresource_get_note($id);
}

function _noteresource_delete($id) {
  noteresource_delete_note($id);
  return (object)array(
    'id' => $id,
  );
}

function _noteresource_index($page, $parameters) {
  global $user;

  $notes = array();
  $res = db_select('note', 'n')
    ->fields('n')
    ->condition('n.uid', $user->uid)
    ->orderBy('modified', 'DESC')
    ->execute();

  foreach ($res as $note) {
    $notes[] = $note;
  }
  return $notes;
}