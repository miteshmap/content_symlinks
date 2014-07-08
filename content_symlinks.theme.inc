<?php

/**
 * Theme nodesymlinks form items.
 */
function theme_content_symlinks_form_items($variables) {
  $form = $variables['form'];
  $output = '<div id="nodesymlinks-items-ajax">';
  $items = element_children($form);
  $rows = array();
  $path = $pathauto = $fragment = FALSE;
  $nid = arg(5);
  $item = content_menu_get_menu_item_from_querystring();
  foreach ($items as $delta) {
    $row = array();
    $row[] = $delta;
    // Always uncheck delete checkboxes.
    $form[$delta]['delete']['#checked'] = FALSE;
    $row[] = drupal_render($form[$delta]['delete']);
    $row[] = drupal_render($form[$delta]['parents']);
    $row[] = drupal_render($form[$delta]['link_title']);
    if (isset($form[$delta]['alias']['pathauto'])) {
      $row[] = drupal_render($form[$delta]['alias']['pathauto']);
      $pathauto = TRUE;
    }
    if (isset($form[$delta]['alias']['path'])) {
      $row[] = drupal_render($form[$delta]['alias']['path']);
      $path = TRUE;
    }
    if (isset($form[$delta]['fragment'])) {
      $row[] = drupal_render($form[$delta]['fragment']);
      $fragment = TRUE;
    }
    $row[] = drupal_render($form[$delta]['weight']);
    $item['link_path'] = 'node/'.$nid.'/mid/'.$form[$delta]['mlid']['#value'];
    $url = 'admin/structure/menu/manage/' . $item['name'] . '/add';
    $row[] = l(t('select'), $url, array('query' => content_menu_assemble_query_string($item)));
    $rows[] = $row;
  }
  $header = array();
  $header[] = '#';
  $header[] = t('Delete');
  $header[] = t('Parent item');
  $header[] = t('Link title');
  if ($pathauto) {
    $header[] = t('Pathauto');
  }
  if ($path) {
    $header[] = t('Path alias');
  }
  if ($fragment) {
    $header[] = t('Fragment');
  }
  $header[] = t('');
  $header[] = t('Weight');
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'nodesymlinks-items')));
  $output .= drupal_render_children($form);
  $output .= '</div>';
  return $output;
}
