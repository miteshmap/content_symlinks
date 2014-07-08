<?php

function content_symlinks_add_symlinks_path($form, &$form_state, $node) {
  module_load_include('inc', 'nodesymlinks', 'nodesymlinks');
  menu_node_prepare($node);
  _nodesymlinks_nodeapi_prepare_presave($node, 'prepare');
  $form['#node'] = $node;

  $form_state['cache'] = TRUE;
  // Add JavaScript that will disable the path textfield
  // when the automatic alias checkbox is checked.
  $form['#attached']['js'][] = drupal_get_path('module', 'nodesymlinks') . '/nodesymlinks.js';

  if (isset($form_state['values']['nodesymlinks']['items'])) {
    $nodesymlinks = (array) $form_state['values']['nodesymlinks']['items'];
    // @TODO: switch to this for PHP 5.3+
    // $nodesymlinks = array_values(array_filter($nodesymlinks, function($x) {return $x['delete'] != 1; }));
    $array_filter_callback = create_function('$x', 'return $x["delete"] != 1;');
    $nodesymlinks = array_values(array_filter($nodesymlinks, $array_filter_callback));
    $form_state['values']['menu']['nodesymlinks']['items'] = $nodesymlinks;
  }
  else {
    $nodesymlinks = empty($node->nodesymlinks) ? NULL : (array) $node->nodesymlinks['items'];
  }

  $form['menu']['nodesymlinks'] = array(
    //'#type' => 'fieldset',
    // @TODO: Fix this.
    '#type' => 'container',
    '#title' => t('Show in More Locations'),
    '#access' => user_access('administer menu') && user_access('administer nodesymlinks'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($nodesymlinks) ? TRUE : FALSE,
    '#tree' => TRUE,
    '#weight' => 1,
    '#attributes' => array('class' => array('nodesymlinks-items-form')),
    '#states' => array(
      'invisible' => array(
        'input[name="menu[enabled]"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['menu']['nodesymlinks']['items'] = array(
    '#tree' => TRUE,
    '#theme' => 'content_symlinks_form_items',
  );

  $delta = 0;
  if (!empty($nodesymlinks)) {
    foreach ($nodesymlinks as $item) {
      $form['menu']['nodesymlinks']['items'][$delta++] = _nodesymlinks_form_field($item, $node);
    }
  }

  if (isset($form_state['nodesymlinks']['item_count'])) {
    $new_max_delta = $form_state['nodesymlinks']['item_count'] - 1;
    for ($delta; $delta <= $new_max_delta; ++$delta) {
      $new_item = array(
        'new_item' => TRUE,
        'link_title' => $node->title,
        'mlid' => 0,
        'alias' => array(),
      );
      $form['menu']['nodesymlinks']['items'][$delta] = _nodesymlinks_form_field($new_item, $node);
    }
  }

  $form['menu']['nodesymlinks']['#description'] = _nodesymlinks_form_description();

  $form['menu']['nodesymlinks']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete locations'),
    '#submit' => array('content_symlinks_node_form_submit_delete'),
    '#ajax' => array(
      'callback' => '_nodesymlinks_form_ajax',
      'wrapper' => 'nodesymlinks-items-ajax',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $form['menu']['nodesymlinks']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add location'),
    '#submit' => array('content_symlinks_node_form_submit_add'),
    '#ajax' => array(
      'callback' => '_nodesymlinks_form_ajax',
      'wrapper' => 'nodesymlinks-items-ajax',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  if (isset($form_state['clicked_button'])) {
    $clicked_val = $form_state['clicked_button']['#value'];
    if ($clicked_val == t('Add Location') || $clicked_val == t('Delete Location(s)')) {
      $form['menu']['enabled']['#default_value'] = TRUE;
      $form['menu']['nodesymlinks']['#collapsed'] = FALSE;
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'submit'
  );

  return $form;
}

/**
 * Add new symlinks item, submit handler for symlinks at node form.
 *
 * @see nodesymlinks_form_alter()
 */
function content_symlinks_node_form_submit_add($form, &$form_state) {
  $form_state['rebuild'] = TRUE;

  // Make the changes we want to the form state.
  if ($form_state['clicked_button']['#value'] == $form['menu']['nodesymlinks']['add']['#value']) {
    $items = element_children($form['menu']['nodesymlinks']['items']);
    // Define new item count - will be used in form process to add new items.
    $form_state['nodesymlinks']['item_count'] = count($items) + 1;
  }
}

/**
* Delete selected symlinks items, submit handler for symlinks at node form.
 *
 * @see nodesymlinks_form_alter()
*/
function content_symlinks_node_form_submit_delete($form, &$form_state) {
  $form_state['rebuild'] = TRUE;

  if ($form_state['clicked_button']['#value'] == $form['menu']['nodesymlinks']['delete']['#value']) {
    $item_keys = element_children($form['menu']['nodesymlinks']['items']);
    $form_state['nodesymlinks']['item_count']  = count($item_keys);
    $values_items = &$form_state['values']['nodesymlinks']['items'];
    $delete_count = 0;
    foreach ($item_keys as $delta) {
      if (!empty($values_items[$delta]['delete'])) {
        $delete_count++;
      }
    }
    $form_state['nodesymlinks']['item_count'] =- $delete_count;

  }
}

function content_symlinks_add_symlinks_path_submit($form, &$form_state) {
  $node = $form['#node'];
  $node->menu['enabled'] = 1;
  $node->menu['nodesymlinks'] = $form_state['values']['nodesymlinks'];
  node_save($node);
}